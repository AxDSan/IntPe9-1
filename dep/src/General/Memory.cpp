#include <General/Memory.h>

MemorySection Memory::searchSection(char* sectionName)
{
	char path[MAX_PATH];
	GetModuleFileNameA(NULL, path, MAX_PATH);
	return Memory::searchSection(path, sectionName);
}

MemorySection Memory::searchSection(char* executablePath, char* sectionName)
{
	FILE* fh = NULL;
	MemorySection ms = {0};
	
	if(fopen_s(&fh, executablePath, "rb"))
		return ms;

	IMAGE_DOS_HEADER imdh;
	IMAGE_NT_HEADERS imnth;
	IMAGE_SECTION_HEADER *pimsh;

	fread(&imdh, sizeof(imdh), 1, fh);
	fseek(fh, imdh.e_lfanew, 0);
	fread(&imnth, sizeof(imnth), 1, fh);
	pimsh = new IMAGE_SECTION_HEADER[imnth.FileHeader.NumberOfSections];
	if(pimsh == NULL)
		return ms;
	fread(pimsh, sizeof(IMAGE_SECTION_HEADER), imnth.FileHeader.NumberOfSections, fh);

	for(uint32 i = 0; i < imnth.FileHeader.NumberOfSections; i++)
	{
		if(!strcmp((char*)pimsh[i].Name, sectionName))
		{
			ms.adress = (uint8*)(pimsh[i].VirtualAddress + imnth.OptionalHeader.ImageBase);
			ms.length = pimsh[i].Misc.VirtualSize;
		}
	}
	delete pimsh;
	fclose(fh);
	return ms;
}

uint8* Memory::searchAddress(MemorySection section, uint8 *signature, uint8 *mask)
{
	return Memory::searchAddress(section.adress, section.length, signature, mask);
}

uint8* Memory::searchAddress(uint8 *begin, uint32 length, uint8 *signature, uint8 *mask)
{
	uint32 signatureLength = strlen((const char*)mask);

	if(signatureLength <= 0 || length <= 0 || begin == NULL)
		return NULL;

	DWORD oldProtection;
	if(!VirtualProtect(begin, length, PAGE_EXECUTE_READWRITE, &oldProtection))
		return NULL;

	for(uint8 *i = begin; i < begin+length; i++)
	{
		if(*(uint8*)i == signature[0])                       //Check if first byte equals
		{
			bool isMatch = true;
			uint32 j;
			for(j = 0; j < signatureLength; j++)
			{
				if(mask[j] != '?')
					if(i[j] != signature[j])
					{
						isMatch = false;
						break;
					}
			}
			if(isMatch)
				return i;
		}
	}
	return NULL;
}

uint8* Memory::searchAddress(MemorySection section, uint8 *signature, uint32 signatureLength)
{
	return Memory::searchAddress(section.adress, section.length, signature, signatureLength);
}

uint8* Memory::searchAddress(uint8 *begin, uint32 length, uint8 *signature, uint32 signatureLength)
{
	if(signatureLength <= 0 || length <= 0 || begin == NULL)
		return NULL;

	DWORD oldProtection;
	if(!VirtualProtect(begin, length, PAGE_EXECUTE_READWRITE, &oldProtection))
		return NULL;
	
	for(uint8 *i = begin; i < begin+length; i++)
		if(*(uint8*)i == signature[0])                            //Check if first byte equals
			if(memcmp(i, signature, signatureLength) == 0)    //Check the whole signature
				return i;                                 //Signature match so return the address
	return NULL;
}

bool Memory::writeJump(uint8 *address, uint8 *toAddress, uint32 nop)
{
	return Memory::writeHook(address, toAddress, JMP, nop);
}

bool Memory::writeCall(uint8 *address, uint8 *toAddress, uint32 nop)
{
	return Memory::writeHook(address, toAddress, CALL, nop);
}

bool Memory::writeHook(uint8 *address, uint8 *toAddress, HookType type, uint32 nop)
{
	DWORD oldProtection;
	if(!VirtualProtect(address, 5 + nop, PAGE_EXECUTE_READWRITE, &oldProtection))
		return false;

	*(uint8*)address = type;                              //Write 1 byte hook type
	*(uint32*)(address+1) = toAddress - (address + 5);    //Calculate offset and write it as 4 bytes
	for(uint32 i = 0; i < nop; i++)
		*(uint8*)(address+5+i) = NOP;                 //Write 1 byte for every nop

	VirtualProtect(address, 5 + nop, oldProtection, &oldProtection);
	return true;
}