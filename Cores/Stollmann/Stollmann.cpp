#include "Stollmann.h"
#include <Windows.h>

#pragma pack(1)
HINSTANCE hLThis = 0;
HINSTANCE hL = 0;
FARPROC p[13] = {0};

Stollmann::Stollmann() : Skeleton()
{


}


void Stollmann::initialize()
{
	if(isGetInfo)
		return;

	hL = LoadLibrary(TARGET_ORIG);

	p[0] = GetProcAddress(hL,"comOpen");
	p[1] = GetProcAddress(hL,"comClose");
	p[2] = GetProcAddress(hL,"comWrite");
	p[3] = GetProcAddress(hL,"comSetProperty");
	p[4] = GetProcAddress(hL,"comGetProperty");
	p[5] = GetProcAddress(hL,"comReset");
	p[6] = GetProcAddress(hL,"comResponse");
	p[7] = GetProcAddress(hL,"debugCaptureOutputEx");
	p[8] = GetProcAddress(hL,"debugGetFlags");
	p[9] = GetProcAddress(hL,"debugSetFlags");
	p[10] = GetProcAddress(hL,"debugCaptureOutput");

	pComWrite = (ComWrite)GetProcAddress(hL,"comWrite");
	DbgPrint("ComWrite at %08X", pComWrite);
	DbgPrint("O my god, imma initialized");
}

void Stollmann::finalize()
{
	if(isGetInfo)
		return;

	FreeLibrary(hL);
}


BOOL FileExists(LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}


void Stollmann::installProxy(const char *myPath)
{
	HKEY hKey;
	char buffer[MAX_PATH];
	char tmp1[MAX_PATH], tmp2[MAX_PATH];

	LONG lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Stollmann\\NFCStack+Eva R02", 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY,  &hKey);
	if(lRet != ERROR_SUCCESS)
		OutputDebugStringA("Could not read the registry");

	RegQueryValue(hKey, "InstallDir", buffer, NULL);

	strcpy(buffer, "C:\\Program Files (x86)\\Stollmann\\NFCStack+Eva R02\\Application");
		 
	OutputDebugStringA(buffer);

	strcpy(tmp1, buffer);
	strcat(tmp1, SLASH);
	strcat(tmp1, TARGET);

	strcpy(tmp2, buffer);
	strcat(tmp2, SLASH);
	strcat(tmp2, TARGET_ORIG);

	OutputDebugStringA(tmp2);

	if(!FileExists(tmp2))
	{
		OutputDebugStringA("Installed NFCStack");
		MoveFile(tmp1, tmp2);
		CopyFile(myPath, tmp1, true);
	}
}

int Stollmann::comWrite(HANDLE h, void* buffer, int size)
{
	DbgPrint("Received a comWrite call");

	MessagePacket *packet = (MessagePacket*)new uint8[size+sizeof(MessagePacket)];
	memcpy(packet->getData(), buffer, size);
	packet->length = size;

	sendMessagePacket(packet);
	delete []packet;
	return pComWrite(h, buffer, size);
}

// comOpen
extern "C" __declspec(naked) void __stdcall comOpen()
	{
	__asm
		{
		jmp p[0*4];
		}
	}

// comClose
extern "C" __declspec(naked) void __stdcall comClose()
	{
	__asm
		{
		jmp p[1*4];
		}
	}

// comWrite
int __stdcall eComWrite(HANDLE h, void* buffer, int size)
{
	return stollmann->comWrite(h, buffer, size);
}

// comSetProperty
extern "C" __declspec(naked) void __stdcall comSetProperty()
	{
	__asm
		{
		jmp p[3*4];
		}
	}

// comGetProperty
extern "C" __declspec(naked) void __stdcall comGetProperty()
	{
	__asm
		{
		jmp p[4*4];
		}
	}

// comReset
extern "C" __declspec(naked) void __stdcall comReset()
	{
	__asm
		{
		jmp p[5*4];
		}
	}

// comResponse
extern "C" __declspec(naked) void __stdcall comResponse()
	{
	__asm
		{
		jmp p[6*4];
		}
	}

// debugCaptureOutputEx
extern "C" __declspec(naked) void __stdcall debugCaptureOutputEx()
	{
	__asm
		{
		jmp p[7*4];
		}
	}

// debugGetFlags
extern "C" __declspec(naked) void __stdcall debugGetFlags()
	{
	__asm
		{
		jmp p[8*4];
		}
	}

// debugSetFlags
extern "C" __declspec(naked) void __stdcall debugSetFlags()
	{
	__asm
		{
		jmp p[9*4];
		}
	}

// debugCaptureOutput
extern "C" __declspec(naked) void __stdcall debugCaptureOutput()
	{
	__asm
		{
		jmp p[10*4];
		}
	}