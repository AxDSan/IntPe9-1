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
