#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

#pragma comment(lib, "psapi.lib")

std::ofstream pFile;

typedef void (__cdecl* ConsoleMsg_)(char* msg);
ConsoleMsg_ oConsoleMsg = NULL;

VOID WINAPIV Log( CHAR* szFormat, ... )
{
	CHAR szBuf[256]; 
	va_list list; 
	va_start( list, szFormat );
	vsprintf_s( szBuf, szFormat, list ); 
	va_end( list ); 

	char szConBuf[256];
	sprintf_s(szConBuf, "%s\n", szBuf);

	oConsoleMsg(szConBuf);
	pFile << szBuf << std::endl;
}

DWORD GetModuleSize(char* module)
{
	MODULEINFO modinfo = {0};
	HMODULE hModule = GetModuleHandle(module);

	if(hModule == 0) {
		return NULL;
	}

	GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(MODULEINFO));
	return modinfo.SizeOfImage;
}

bool bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
    for(;*szMask;++szMask,++pData,++bMask)
    {
        if(*szMask=='x' && *pData!=*bMask)
        {
            return false;
        }
    }
    return (*szMask) == NULL;
}

DWORD dwFindPattern(DWORD dwAddress,DWORD dwLen,BYTE *bMask,char * szMask)
{
    for(DWORD i=0; i < dwLen; i++)
    {
        if(bDataCompare((BYTE*)(dwAddress+i ),bMask,szMask))
        {
            return (DWORD)(dwAddress+i);
        }
    }
    return 0;
}