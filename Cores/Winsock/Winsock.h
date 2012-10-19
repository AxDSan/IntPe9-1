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
#ifndef WINSOCK_H
#define WINSOCK_H

#include <Skeleton.h>
#include "General/Memory.h"

// Static addresses
extern uint32 addressSend;
extern uint32 addressRecv;
extern uint32 addressWSASend;
extern uint32 addressWSARecv;
extern uint32 addressWSASendTo;
extern uint32 addressWSARecvFrom;

// Forward decelerations
void CaveSend();
void CaveRecv();
void CaveWSASend();
void CaveWSARecv();
void CaveWSASendTo();
void CaveWSARecvFrom();

// Offsets for ws2_32.dll 
#define OFFSET_JMP 5
#define OFFSET_RECV 0x97
#define OFFSET_WSARECV 0x95
#define OFFSET_WSARECVFROM 0x8F

class Winsock : public Skeleton
{
public:
	Winsock();

	void initialize();
	void finalize();

	void parsePython(const char *script){};
	bool installProxy(const char *myPath){return false;};
};

extern Winsock *winsock;

#endif