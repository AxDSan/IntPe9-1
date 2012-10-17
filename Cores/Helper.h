#ifndef HELPER_H
#define HELPER_H

#include <Windows.h>
#include <stdio.h>
#include "Crypto/MurmurHash3.h"
#include "typedef.h"

namespace Helper
{
	bool FileExists(const char *path);
	uint32 getHash(HANDLE file);
	bool IsNewVersion(const char *pathLeft, const char *pathRight);
}

#endif