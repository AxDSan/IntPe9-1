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

bool doFirst = true;
MessagePacket *sendBuf;
MessagePacket *recvBuf;

// CodeCave addresses
uint32 addressSend = 0;
uint32 addressRecv = 0;

Winsock::Winsock()
{

}

void Winsock::initialize()
{
	// Guard
	if(isGetInfo)
		return;

	// First create buffers!!! then hook
	sendBuf = (MessagePacket*)new uint8[MP_MAX_SIZE];
	recvBuf = (MessagePacket*)new uint8[MP_MAX_SIZE];
	
	// Hook WSA by IAT
	_oldWSASendTo = (defWSASendTo)_upx->hookIatFunction("ws2_32", "WSASendTo", (unsigned long)&newWSASendTo);
	_oldWSARecvFrom = (defWSARecvFrom)_upx->hookIatFunction("ws2_32", "WSARecvFrom", (unsigned long)&newWSARecvFrom);
	_oldWSASend = (defWSASend)_upx->hookIatFunction("ws2_32", "WSASend", (unsigned long)&newWSASend);
	_oldWSARecv = (defWSARecv)_upx->hookIatFunction("ws2_32", "WSARecv", (unsigned long)&newWSARecv);
	_oldSend = (defSend)_upx->hookIatFunction("ws2_32", "send", (unsigned long)&newSend);
	_oldRecv = (defRecv)_upx->hookIatFunction("ws2_32", "recv", (unsigned long)&newRecv);

	// Now check for every function if we succeeded and if not lets do some inline hooking
	try
	{
		if(!_oldSend)
		{
			addressSend = ((uint32)&send) + 5;
			DbgPrint("Hooking send by inline: %08X", send);
			if(send)
				Memory::writeJump((uint8*)send, (uint8*)CaveSend);
		}
		if(!_oldRecv)
		{
			addressRecv = ((uint32)&recv) + 0x97;
			DbgPrint("Hooking recv by inline: %08X", addressRecv);
			if(recv)
				Memory::writeJump((uint8*)addressRecv, (uint8*)CaveRecv, 1);
		}
		if(!_oldWSARecv)
		{
			DbgPrint("Hooking WSARecv inline: %08X", WSARecv);
		}
	}
	catch(...)
	{
		DbgPrint("Something went terrible wrong with inline hooks");
	}

	start();
	DbgPrint("Winsock core started!");
}

void Winsock::finalize()
{
	// Guard
	if(isGetInfo)
		return;

	DbgPrint("Terminating winsock core");

	exit();

	delete[]sendBuf;
	delete[]recvBuf;
}

// CodeCave wrappers
__declspec(naked) void CaveSend()
{
	__asm
	{
		// Restore prolog
		mov edi, edi
		push ebp
		mov ebp, esp

		// Push stack and call our function
		pushad
		push [ebp+0x14]
		push [ebp+0x10]
		push [ebp+0xC]
		push [ebp+0x8]
		call newSend
		popad

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

// HOOK wrappers
int WSAAPI newSend(SOCKET s, const char *buf, int len, int flags)
{
	winsock->DbgPrint("Send with len:%i on socket: %08X", len, s);

	// Get data and send it to front end
	//sendBuf->reset();
	sendBuf->type = SEND;
	sendBuf->length = len;
	memcpy(sendBuf->getData(), buf, sendBuf->length);
	winsock->sendMessagePacket(sendBuf);
	
	if(addressSend) //Inline hook so let codecave jump back
		return 0;
	winsock->DbgPrint("Old call");
	return winsock->_oldSend(s, buf, len, flags);
}

int WSAAPI newRecv(SOCKET s, char *buf, int len, int flags)
{
	winsock->DbgPrint("Recv IAT hook");
	int bytesRecved = winsock->_oldRecv(s, buf, len, flags);
	inlineRecv(s, buf, len, flags, bytesRecved);
	return bytesRecved;
}

void WSAAPI inlineRecv(SOCKET s, char *buf, int len, int flags, int bytesRecved)
{
	winsock->DbgPrint("Recv with len:%i (b: %i) on socket: %08X", len, bytesRecved, s);
	if(bytesRecved <= 0)
		return;

	winsock->DbgPrint("Recv with len:%i on socket: %08X", len, s);

	// Get data and send it to front end
	sendBuf->reset();
	sendBuf->type = RECV;
	sendBuf->length = bytesRecved;
	memcpy(sendBuf->getData(), buf, sendBuf->length);
	winsock->sendMessagePacket(sendBuf);
}

int WSAAPI newWSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	winsock->DbgPrint("WSASend");

	for(uint32 i = 0; i <= dwBufferCount; i++)
	{
		sendBuf->reset();
		sendBuf->type = WSASEND;
		sendBuf->length = lpBuffers[0].len;
		memcpy(sendBuf->getData(), lpBuffers[0].buf, sendBuf->length);
		winsock->sendMessagePacket(sendBuf);
	}

	return winsock->_oldWSASend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
}

int WSAAPI newWSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	int returnLength = winsock->_oldWSARecv(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);

	winsock->DbgPrint("WSARecv");

	if(returnLength == 0)
	{
		sendBuf->reset();
		recvBuf->type = WSARECV;
		recvBuf->length = lpBuffers[0].len;
		memcpy(recvBuf->getData(), lpBuffers[0].buf, recvBuf->length);
		winsock->sendMessagePacket(recvBuf);
	}

	return returnLength;
}

int WSAAPI newWSASendTo(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr *lpTo, int iToLen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	winsock->DbgPrint("WSASendTo");

	for(uint32 i = 0; i <= dwBufferCount; i++)
	{
		sendBuf->reset();
		sendBuf->type = WSASENDTO;
		sendBuf->length = lpBuffers[0].len;
		memcpy(sendBuf->getData(), lpBuffers[0].buf, sendBuf->length);
		winsock->sendMessagePacket(sendBuf);
	}
	return winsock->_oldWSASendTo(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpTo, iToLen, lpOverlapped, lpCompletionRoutine);
}

int WSAAPI newWSARecvFrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	winsock->DbgPrint("WSARecvFrom");
	int returnLength = winsock->_oldWSARecvFrom(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpFrom, lpFromlen, lpOverlapped, lpCompletionRoutine);

	if(returnLength == 0)
	{
		sendBuf->reset();
		recvBuf->type = WSARECVFROM;
		recvBuf->length = lpBuffers[0].len;
		memcpy(recvBuf->getData(), lpBuffers[0].buf, recvBuf->length);
		winsock->sendMessagePacket(recvBuf);
	}

	return returnLength;
}