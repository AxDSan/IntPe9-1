#ifndef MEMORY_H
#define MEMORY_H

#include <Windows.h>
#include <stdio.h>

typedef long long          int64;
typedef int                int32;
typedef short              int16;
typedef char               int8; 
typedef unsigned long long uint64;
typedef unsigned int       uint32;
typedef unsigned short     uint16;
typedef unsigned char      uint8;

struct MemorySection
{
	uint8* adress;
	uint32 length;
};

namespace Memory
{
	enum HookType : uint8
	{
		JMP = 0xE9,
		CALL = 0xE8,
		NOP = 0x90
	};

	MemorySection searchSection(char* sectionName);
	MemorySection searchSection(char* executablePath, char* sectionName);

	uint8* searchAddress(MemorySection section, uint8 *signature, uint8 *mask);
	uint8* searchAddress(uint8 *begin, uint32 length, uint8 *signature, uint8 *mask);
	uint8* searchAddress(MemorySection section, uint8 *signature, uint32 signatureLength);
	uint8* searchAddress(uint8 *begin, uint32 length, uint8 *signature, uint32 signatureLength);

	bool writeJump(uint8 *address, uint8 *toAddress, uint32 nop = 0);
	bool writeCall(uint8 *address, uint8 *toAddress, uint32 nop = 0);
	bool writeHook(uint8 *address, uint8 *toAddress, HookType type = CALL, uint32 nop = 0);
}

#endif