/*
IntPe9 a open source multi game, general, all purpose and educational packet editor
Copyright (C) 2012  Intline9 <Intline9@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "Stollmann.h"

FARPROC proc[7];
HANDLE comHandle = NULL;
uint32 returnAddress;

BOOST_PYTHON_MODULE(stollman)
{
	boost::python::class_<Stollmann>("sm")
		.def("send", &Stollmann::send);

	def("getInstance", &Stollmann::getInstance, boost::python::return_value_policy<boost::python::reference_existing_object>());
}

Stollmann::Stollmann() : Skeleton()
{


}

void Stollmann::initialize()
{
	// Guard
	if(isGetInfo)
		return;

	DbgPrint("Loading original library");
	dllHandle = LoadLibrary(TARGET_ORIG);

	// Original call table
	proc[0] = GetProcAddress(dllHandle,"comOpen");
	proc[1] = GetProcAddress(dllHandle,"comClose");
	proc[2] = GetProcAddress(dllHandle,"comWrite");
	proc[3] = GetProcAddress(dllHandle,"comSetProperty");
	proc[4] = GetProcAddress(dllHandle,"comGetProperty");
	proc[5] = GetProcAddress(dllHandle,"comReset");
	proc[6] = GetProcAddress(dllHandle,"comResponse");

	// Get hooks addresses
	pComWrite = (ComWrite)GetProcAddress(dllHandle,"comWrite");

	// Print all hooks
	DbgPrint("ComWrite at %08X", pComWrite);

	// Install custom hook
	uint32 baseAddress = (uint32)dllHandle;
	uint32 transportAddress = baseAddress+0x15E0;
	returnAddress = baseAddress+0x15E0+5;
	DbgPrint("Image base of dll is: %08X, transportEvent: %08X", baseAddress, transportAddress);
	Memory::writeJump((uint8*)(transportAddress), (uint8*)transportEvent, 2); // From the base of the dll offset 0x251A

	// Start the skeleton
	start();

	// Start command listener + python
	startThread();

	// Extend python
	initstollman();
	boost::python::object mainModule = boost::python::import("__main__");
	pythonNamespace = mainModule.attr("__dict__");
	PyRun_SimpleString("import stollman");
	PyRun_SimpleString("import cStringIO");
	PyRun_SimpleString("sm = stollman.getInstance()");
	PyRun_SimpleString("sys.stderr = cStringIO.StringIO()");
}

void Stollmann::finalize()
{
	if(isGetInfo)
		return;

	FreeLibrary(dllHandle);

	exit();
}

Stollmann *Stollmann::getInstance()
{
	return stollmann;
}

bool Stollmann::installProxy(const char *myPath)
{
	HKEY hKey;
	char installPath[MAX_PATH];
	char proxy[MAX_PATH], original[MAX_PATH];

	long lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Stollmann\\NFCStack+Eva R04", 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY,  &hKey);
	if(lRet != ERROR_SUCCESS)
	{
		DbgExport("Could not read the registry (error %i)", lRet);
		return false;
	}

	DWORD size = MAX_PATH;
	lRet = RegQueryValueEx(hKey, "InstallDir", 0, NULL, (LPBYTE)installPath, &size);
	RegCloseKey(hKey);
	if(lRet != ERROR_SUCCESS)
	{
		DbgExport("Could not read the key (error %i) (size: %i)", lRet, size);
		return false;
	}

	sprintf_s(proxy, MAX_PATH, "%s\\%s\\%s", installPath, APP_DIR, TARGET);
	sprintf_s(original, MAX_PATH, "%s\\%s\\%s", installPath, APP_DIR, TARGET_ORIG);

	// Not installed yet so just install it
	if(!Helper::FileExists(original))
	{
		DbgExport("Installing %s proxy dll", name);
		MoveFile(proxy, original);
		CopyFile(myPath, proxy, true);

		DbgExport("Target dll: %s", proxy);
		DbgExport("Backup dll: %s", original);
	}
	else
	{
		if(Helper::IsNewVersion(myPath, proxy)) // Is this a different version, then we overwrite
		{
			DbgExport("Installing updated version %i.%i of %s", versionNo.major, versionNo.minor, name);
			CopyFile(myPath, proxy, false);
		}
	}
	return true;
}

// Python extensions
void Stollmann::parsePython(const char *script)
{
	try
	{
		exec(script, pythonNamespace);
	}
	catch(boost::python::error_already_set const &)
	{
		PyErr_Print();
		boost::python::object sys(boost::python::handle<>(PyImport_ImportModule("sys")));
		boost::python::object err = sys.attr("stderr");
		std::string errorText = boost::python::extract<std::string>(err.attr("getvalue")());
		DbgPrint(errorText.c_str());
		PyRun_SimpleString("sys.stderr = cStringIO.StringIO()");
	}
}

uint8 *Stollmann::listToChars(boost::python::list &pList, uint32 *size)
{
	boost::python::stl_input_iterator<uint8> begin(pList), end;
	std::list<uint8>bytes;
	bytes.assign(begin, end);

	*size = bytes.size();
	uint8 *buffer;
	buffer = new uint8[bytes.size()];

	uint32 i = 0;
	while(!bytes.empty())
	{
		buffer[i] = bytes.front();
		bytes.pop_front(); i++;
	}
	return buffer;
}

void Stollmann::send(boost::python::list &bytes)
{
	if(comHandle == NULL)
		return;

	uint32 length;
	uint8 *packet = listToChars(bytes, &length);
	DbgPrint("Wrote %i bytes to com: %08X", length, comHandle);
	pComWrite(comHandle, packet, length);
	delete []packet;
}

// Stollmann hooks
int Stollmann::comWrite(HANDLE h, void* buffer, int size)
{
	MessagePacket *packet = (MessagePacket*)new uint8[size+sizeof(MessagePacket)];
	
	if(comHandle == NULL)
		comHandle = h;

	DbgPrint("Com write, handle: %08X", h);
	memcpy(packet->getData(), buffer, size);
	packet->length = size;
	packet->description[0] = '\0';
	packet->type = SEND;

	sendMessagePacket(packet);

	delete []packet;
	return pComWrite(h, buffer, size);
}

void Stollmann::comTransport(HANDLE h, int eventNo, bool a3, void *buffer, uint32 size)
{
	if(eventNo != 3)
		return;

	MessagePacket *packet = (MessagePacket*)new uint8[size+sizeof(MessagePacket)];

	DbgPrint("Com read, handle: %08X", h);
	memcpy(packet->getData(), buffer, size);
	packet->length = size;
	packet->description[0] = '\0';
	packet->type = RECV;

	sendMessagePacket(packet);

	delete []packet;
}

int __stdcall transportEvent(int a1, int eventNo, int a3, void *buffer, size_t size)
{
	stollmann->DbgPrint("Transport event (%i): %08X, %i, buffer: %08X, len: %i", eventNo, a1, a3, buffer, size);
	stollmann->comTransport((HANDLE)a1, eventNo, a3, buffer, size);
	// Do not use this epilogue, instead jump to original transport event
	__asm
	{
		mov esi,[ebp+0xC]
		jmp returnAddress;
	}
}

// Wrappers
int __stdcall eComWrite(HANDLE h, void* buffer, int size)
{
	return stollmann->comWrite(h, buffer, size);
}

// Proxy
extern "C" __declspec(naked) void __stdcall comOpen()
{
	__asm
	{
		jmp proc[0*4];
	}
}

extern "C" __declspec(naked) void __stdcall comClose()
{
	__asm
	{
		jmp proc[1*4];
	}
}

extern "C" __declspec(naked) void __stdcall comWrite()
{
	__asm
	{
		jmp proc[2*4];
	}
}

extern "C" __declspec(naked) void __stdcall comSetProperty()
{
	__asm
	{
		jmp proc[3*4];
	}
}

extern "C" __declspec(naked) void __stdcall comGetProperty()
{
	__asm
	{
		jmp proc[4*4];
	}
}

extern "C" __declspec(naked) void __stdcall comReset()
{
	__asm
	{
		jmp proc[5*4];
	}
}

extern "C" __declspec(naked) void __stdcall comResponse()
{
	__asm
	{
		jmp proc[6*4];
	}
}