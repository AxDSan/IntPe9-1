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

	// Original call table
	p[0] = GetProcAddress(hL,"comCallControl");
	p[1] = GetProcAddress(hL,"comClose");
	p[2] = GetProcAddress(hL,"comEnumAdapters");
	p[3] = GetProcAddress(hL,"comGetInterface");
	p[4] = GetProcAddress(hL,"comGetProperty");
	p[5] = GetProcAddress(hL,"comOpen");
	p[6] = GetProcAddress(hL,"comReset");
	p[7] = GetProcAddress(hL,"comSetProperty");
	p[8] = GetProcAddress(hL,"comWrite");
	p[9] = GetProcAddress(hL,"debugCaptureOutput");
	p[10] = GetProcAddress(hL,"debugCaptureOutputEx");
	p[11] = GetProcAddress(hL,"debugGetFlags");
	p[12] = GetProcAddress(hL,"debugSetFlags");

	// Get hooks addresses
	pComWrite = (ComWrite)GetProcAddress(hL,"comWrite");

	// Print all hooks
	DbgPrint("ComWrite at %08X", pComWrite);

	// Install custom hook
	DbgPrint("Image base of dll is: %08X, hook address: %08X", hL, (hL+0x251A));
	Memory::writeCall((uint8*)(hL+0x251A), (uint8*)codeCave, 0); // From the base of the dll offset 0x251A

	// Start the skeleton
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

void Stollmann::comResponse(void *buffer, uint32 bufferSize, uint32 size)
{
	if(size <= 0)
		return;

	DbgPrint("Response received with size %i", size);
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

// Hooks
__declspec(naked) void codeCave()
{
	__asm
	{
		mov ecx, eax         // Place lpBuffer in ecx
		call ReadFile
		push eax             // Store response of ReadFile
		mov eax, [esi+0x364]
		push eax             // (lp)NumberOfBytesRead
		push edx             // nNumberOfBytesToRead
		push ecx             // lpBuffer
		call ComResponse
		pop eax
		ret
	}
}

// Wrappers
void ComResponse(void *buffer, uint32 bufferSize, uint32 size)
{
	stollmann->comResponse(buffer, bufferSize, size);
}

int __stdcall eComWrite(HANDLE h, void* buffer, int size)
{
	return stollmann->comWrite(h, buffer, size);
}

// ORIGINALS
extern "C" __declspec(naked) void __stdcall comCallControl()
	{
	__asm
		{
		jmp p[0*4];
		}
	}

extern "C" __declspec(naked) void __stdcall comClose()
	{
	__asm
		{
		jmp p[1*4];
		}
	}

extern "C" __declspec(naked) void __stdcall comEnumAdapters()
	{
	__asm
		{
		jmp p[2*4];
		}
	}

extern "C" __declspec(naked) void __stdcall comGetInterface()
	{
	__asm
		{
		jmp p[3*4];
		}
	}

extern "C" __declspec(naked) void __stdcall comGetProperty()
	{
	__asm
		{
		jmp p[4*4];
		}
	}

extern "C" __declspec(naked) void __stdcall comOpen()
{
	__asm
	{
		jmp p[5*4];
	}
}

extern "C" __declspec(naked) void __stdcall comReset()
{
	__asm
	{
		jmp p[6*4];
	}
}

extern "C" __declspec(naked) void __stdcall comSetProperty()
{
	__asm
	{
		jmp p[7*4];
	}
}

extern "C" __declspec(naked) void __stdcall comWrite()
{
	__asm
	{
		jmp p[8*4];
	}
}

extern "C" __declspec(naked) void __stdcall debugCaptureOutput()
{
	__asm
	{
		jmp p[9*4];
	}
}

extern "C" __declspec(naked) void __stdcall debugCaptureOutputEx()
{
	__asm
	{
		jmp p[10*4];
	}
}

extern "C" __declspec(naked) void __stdcall debugGetFlags()
{
	__asm
	{
		jmp p[11*4];
	}
}

extern "C" __declspec(naked) void __stdcall debugSetFlags()
{
	__asm
	{
		jmp p[12*4];
	}
}