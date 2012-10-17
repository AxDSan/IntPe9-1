#include "Helper.h"

bool Helper::FileExists(const char *path)
{
	DWORD dwAttrib = GetFileAttributes(path);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

uint32 Helper::getHash(HANDLE file)
{
	uint32 hash = 42;
	DWORD byteNo;

	uint32 size = GetFileSize(file, NULL);
	void *data = malloc(size);

	if(ReadFile(file, data, size, &byteNo, NULL))
		MurmurHash3_x86_32(data, size, 42, &hash);
	return hash;
}

bool Helper::IsNewVersion(const char *pathLeft, const char *pathRight)
{
	HANDLE hLeft = CreateFile(pathLeft, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hRight = CreateFile(pathRight, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hLeft == NULL || hRight == NULL)
		return false; // Dafuq, should not happen??

	// I do not think we are comparing 4GB+ files
	uint32 hashLeft = getHash(hLeft);
	uint32 hashRight = getHash(hRight);

	// Close
	CloseHandle(hLeft);
	CloseHandle(hRight);

	return (hashLeft != hashRight);
}
