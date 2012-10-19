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
#include "Winsock.h"

// CodeCave wrappers
__declspec(naked) void CaveSend()
{
	__asm
	{
		// Restore prolog
		mov edi, edi
		push ebp
		mov ebp, esp

		// call our function
		push eax
		push [ebp+0x14]
		push [ebp+0x10]
		push [ebp+0xC]
		push [ebp+0x8]
		call newSend
		pop eax

		// Return with send
		jmp addressSend
	}
}

__declspec(naked) void CaveRecv()
{
	__asm
	{
		push eax // Eax contains ret bytes so save it
		push eax
		push [ebp+0x14]
		push [ebp+0x10]
		push [ebp+0x0C]
		push [ebp+0x08]
		call inlineRecv
		pop eax

		// Restore bytes 
		pop esi
		pop ebx
		leave 
		ret 16
	}
}

__declspec(naked) void CaveWSASend()
{
	__asm
	{
		// Restore prolog
		mov edi, edi
		push ebp
		mov ebp, esp

		// call our function
		push eax
		push [ebp+0x20]
		push [ebp+0x1C]
		push [ebp+0x18]
		push [ebp+0x14]
		push [ebp+0x10]
		push [ebp+0x0C]
		push [ebp+0x08]
		call newWSASend
		pop eax

		// Return with send
		jmp addressWSASend
	}
}

__declspec(naked) void CaveWSARecv()
{
	__asm
	{
		// Only call our hook function if there was no error
		cmp eax, 0
		jne finish

		push eax //Contains state
		push [ebp+0x20]
		push [ebp+0x1C]
		push [ebp+0x18]
		push [ebp+0x14]
		push [ebp+0x10]
		push [ebp+0x0C]
		push [ebp+0x08]
		call newWSARecv
		pop eax

		// Restore bytes
		finish:
		pop esi
		leave
		retn 1
	}
}

__declspec(naked) void CaveWSASendTo()
{
	__asm
	{
		// Restore prolog
		mov edi, edi
		push ebp
		mov ebp, esp

		// call our function
		push eax
		push [ebp+0x28]
		push [ebp+0x24]
		push [ebp+0x20]
		push [ebp+0x1C]
		push [ebp+0x18]
		push [ebp+0x14]
		push [ebp+0x10]
		push [ebp+0x0C]
		push [ebp+0x08]
		call newWSASendTo
		pop eax

		// Return with send
		jmp addressWSASendTo
	}
}

__declspec(naked) void CaveWSARecvFrom()
{
	__asm
	{
		// Only call our hook function if there was no error
		cmp eax, 0
		jne finish

		push eax //Contains state
		push [ebp+0x28]
		push [ebp+0x24]
		push [ebp+0x20]
		push [ebp+0x1C]
		push [ebp+0x18]
		push [ebp+0x14]
		push [ebp+0x10]
		push [ebp+0x0C]
		push [ebp+0x08]
		call newWSARecvFrom
		pop eax

		// Restore bytes
		finish:
		pop esi
		leave
		retn 0x24
	}
}