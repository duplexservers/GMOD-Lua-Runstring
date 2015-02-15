#include <windows.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <string>

#include "header.h"
#include "class.h"

#include "detours.h"
#pragma comment(lib, "detours.lib")

extern std::ofstream pFile;


DWORD dwBase					= NULL;
HMODULE hTier0					= NULL;

DWORD dwMenuSystem				= NULL;
DWORD dwMenuSystem_Size			= NULL;

DWORD dwLua						= NULL;
DWORD dwMenuLua					= NULL;

CLUAManager * LUAManager = NULL;
CStack * stack = NULL;

void Init()
{
	pFile.open("C://GMOD//debug.log", std::ios::app);

	dwMenuLua = dwFindPattern(dwMenuSystem, dwMenuSystem_Size, (PBYTE)"\x6A\x01\x6A\x01\x6A\x01\xFF\xD2\x8B\x0D\x00\x00\x00\x00\x50\x8B\x86\x00\x00\x00\x00\x68\x00\x00\x00\x00\x68\x00\x00\x00\x00", "xxxxxxxxxx????xxx????x????x????") + 0xA;

	while(*(DWORD*)*(DWORD*)dwMenuLua == NULL)
		Sleep(100);

	Log("GMOD Lua Runstring: %x", *(DWORD*)*(DWORD*)*(DWORD*)dwMenuLua + 0x160);

	dwLua = *(DWORD*)*(DWORD*)*(DWORD*)dwMenuLua + 0x160;
	LUAManager = (CLUAManager*)(*(DWORD*)*(DWORD*)dwMenuLua);
}

typedef int (__cdecl* _RunScriptHook)();
_RunScriptHook oRunScriptHook = NULL;

void __declspec(naked) RunScriptHook()
{
	__asm pushad
	__asm mov stack, esp

	stack->szLua;

	Log(stack->szLua);

	__asm popad
	__asm jmp[oRunScriptHook];
}



DWORD WINAPI LoopFunction( LPVOID lpParm ) 
{
	for (;dwBase == NULL;Sleep(100))
		dwBase = (DWORD)GetModuleHandle("hl2.exe");

	//Load ConsoleMsg
	hTier0 = LoadLibrary("tier0.dll");
	oConsoleMsg = (ConsoleMsg_)(DWORD)GetProcAddress(hTier0, "Msg");

	for (;dwMenuSystem == NULL;Sleep(100))
		dwMenuSystem = (DWORD)GetModuleHandle("menusystem.dll");

	for (;dwMenuSystem_Size == NULL;Sleep(100))
		dwMenuSystem_Size = (DWORD)GetModuleSize("menusystem.dll");

	Init();

	oRunScriptHook = (_RunScriptHook)DetourFunction((PBYTE)*(DWORD*)dwLua, (PBYTE)RunScriptHook);

	return 0;
}

BOOL WINAPI DllMain ( HMODULE hModule, DWORD dwReason, LPVOID lpReserved )
{
	if ( dwReason == DLL_PROCESS_ATTACH )	
		CreateThread(0, 0, LoopFunction, 0, 0, 0);	

	return TRUE;
}