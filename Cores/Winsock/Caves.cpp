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
/*
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
			jmp addressSend2
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
		call inlineRecv2
			pop eax

			// Restore bytes 
			pop esi
			pop ebx
			leave 
			ret 0x10
	}
}

__declspec(naked) void CaveSendTo()
{
	__asm
	{
		// Restore prolog
		mov edi, edi
			push ebp
			mov ebp, esp

			// call our function
			push eax
			push [ebp+0x1C]
		push [ebp+0x18]
		push [ebp+0x14]
		push [ebp+0x10]
		push [ebp+0xC]
		push [ebp+0x8]
		call newSendTo
			pop eax

			// Return with send
			jmp addressSendTo2
	}
}*/

__declspec(naked) void CaveRecvFrom()
{
	__asm
	{
		mov edx, [ebp+0x10] // NumberOfBytesRecvd
		cmp edx, 0xFFFFFFFF         
		je finish           // Skip if there is nothing received

		mov isRecvFrom, 1   // Disable WSARecvFrom
		push 0
		push 0
		push [ebp+0x1C]     // lpFromlen
		push [ebp+0x18]     // lpFrom
		push [ebp+0x0C]     // Flags
		push 1              // dwBufferCount
		push [ebp-0x08]     // Buffers
		push [ebp+0x08]     // Socket
		call newWSARecvFrom

		// Restore bytes
		finish:
		mov eax, [ebp+0x10]
		leave
		retn 0x18
	}
}
__declspec(naked) void CaveSend2()
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
		jmp addressSend2
	}
}

__declspec(naked) void CaveRecv2()
{
	__asm
	{
		push eax // Eax contains ret bytes so save it
		push eax
		push [ebp+0x14]
		push [ebp+0x10]
		push [ebp+0x0C]
		push [ebp+0x08]
		call inlineRecv2
		pop eax

		// Restore bytes 
		pop esi
		pop ebx
		leave 
		ret 0x10
	}
}

__declspec(naked) void CaveSendTo2()
{
	__asm
	{
		// Restore prolog
		mov edi, edi
		push ebp
		mov ebp, esp

		// call our function
		push eax
		push [ebp+0x1C]
		push [ebp+0x18]
		push [ebp+0x14]
		push [ebp+0x10]
		push [ebp+0xC]
		push [ebp+0x8]
		call newSendTo
		pop eax

		// Return with send
		jmp addressSendTo2
	}
}

__declspec(naked) void CaveRecvFrom2()
{
	__asm
	{
		push eax // Eax contains ret bytes so save it
		push eax
		push [ebp+0x1C]
		push [ebp+0x18]
		push [ebp+0x14]
		push [ebp+0x10]
		push [ebp+0x0C]
		push [ebp+0x08]
		call inlineRecvFrom2
		pop eax

		// Restore bytes 
		pop esi
		pop ebx
		leave 
		ret 0x18
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