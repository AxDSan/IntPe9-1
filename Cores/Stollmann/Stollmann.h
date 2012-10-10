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
#ifndef STOLLMAN_H
#define STOLLMAN_H

#include <Skeleton.h>
#include <windows.h>

#define SLASH "\\"
#define APP_DIR "Application"
#define TARGET_ORIG "ehciTransport.orig.dll"
#define TARGET "ehciTransport.dll"

//Typedef
typedef int (__stdcall *ComWrite)(HANDLE h, void* buffer, int size);
typedef int (__stdcall *ComResponse)(HANDLE h, void* data);

class Stollmann : public Skeleton
{
private:


public:
	Stollmann();

	void initialize();
	void finalize();

	bool installProxy(const char *myPath);

	// Proxy
	int comWrite(HANDLE h, void* buffer, int size);
	int comResponse(HANDLE h, void* data);

	// Proxy old functions
	ComWrite pComWrite;
	ComResponse pComResponse;
};

extern Stollmann *stollmann;

#endif