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

	DbgPrint("Loading original library");
	hL = LoadLibrary(TARGET_ORIG);

	p[0] = GetProcAddress(hL,"comOpen");
	p[1] = GetProcAddress(hL,"comClose");
	p[2] = GetProcAddress(hL,"comWrite");
	p[3] = GetProcAddress(hL,"comSetProperty");
	p[4] = GetProcAddress(hL,"comGetProperty");
	p[5] = GetProcAddress(hL,"comReset");
	p[6] = GetProcAddress(hL,"comResponse");

	pComWrite = (ComWrite)GetProcAddress(hL,"comWrite");
	DbgPrint("ComWrite at %08X", pComWrite);

	start();
}

void Stollmann::finalize()
{
	if(isGetInfo)
		return;

	FreeLibrary(hL);

	exit();
}

bool Stollmann::installProxy(const char *myPath)
{
	HKEY hKey;
	char installPath[MAX_PATH*2];
	char proxy[MAX_PATH], original[MAX_PATH];

	long lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Stollmann\\NFCStack+Eva R04", 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY,  &hKey);
	if(lRet != ERROR_SUCCESS)
	{
		DbgExport("Could not read the registry (error %i)", lRet);
		return false;
	}

	DWORD size;
	lRet = RegQueryValueEx(hKey, "InstallDir", 0, NULL, (LPBYTE)installPath, &size);
	RegCloseKey(hKey);
	if(lRet != ERROR_SUCCESS)
	{
		DbgExport("Could not read the key (error %i)", lRet);
		return false;
	}

	sprintf_s(proxy, MAX_PATH, "%s\\%s\\%s", installPath, APP_DIR, TARGET);
	sprintf_s(original, MAX_PATH, "%s\\%s\\%s", installPath, APP_DIR, TARGET_ORIG);

	// Not installed yet so just install it
	if(!FileExists(original))
	{
		DbgExport("Installing %s proxy dll", name);
		MoveFile(proxy, original);
		CopyFile(myPath, proxy, true);

		DbgExport("Target dll: %s", proxy);
		DbgExport("Backup dll: %s", original);
	}
	else
	{
		if(IsNewVersion(myPath, proxy)) // Is this a different version, then we overwrite
		{
			DbgExport("Installing updated version %i.%i of %s", versionNo.major, versionNo.minor, name);
			CopyFile(myPath, proxy, false);
		}
	}
	return true;
}

int Stollmann::comWrite(HANDLE h, void* buffer, int size)
{
	MessagePacket *packet = (MessagePacket*)new uint8[size+sizeof(MessagePacket)];
	
	memcpy(packet->getData(), buffer, size);
	packet->length = size;
	packet->description[0] = '\0';
	packet->type = SEND;

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