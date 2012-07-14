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

char *Winsock::getName()
{
	return "Winsock.dll";
}

Winsock::Winsock()
{

}

void Winsock::initialize()
{
	//First create buffers!!! then hook
	sendBuf = (MessagePacket*)new uint8[MP_MAX_SIZE];
	recvBuf = (MessagePacket*)new uint8[MP_MAX_SIZE];
	
	
	//_oldWSASendTo = (defWSASendTo)_upx->hookIatFunction("ws2_32", "WSASendTo", (unsigned long)&newWSASendTo);
	//_oldWSARecvFrom = (defWSARecvFrom)_upx->hookIatFunction("ws2_32", "WSARecvFrom", (unsigned long)&newWSARecvFrom);
	//_oldWSASend = (defWSASend)_upx->hookIatFunction("ws2_32", "WSASend", (unsigned long)&newWSASend);
	//_oldWSARecv = (defWSARecv)_upx->hookIatFunction(NULL, "WSARecv", (unsigned long)&newWSARecv);
	_oldSend = (defSend)_upx->hookIatFunction(NULL, "send", (unsigned long)&newSend);


	DbgPrint("Original WSASend: %08X, New WSASend: %08X", _oldWSASend, &newWSASend);
	DbgPrint("Original send: %08X, New send: %08X, real: %08X", _oldSend, &newSend, &send);
	DbgPrint("Winsock started!");
}

void Winsock::finalize()
{
	delete[]sendBuf;
	delete[]recvBuf;
}

int WSAAPI newSend(SOCKET s, const char FAR *buf, int len, int flags)
{
	winsock->DbgPrint("Send");
	return winsock->_oldSend(s, buf, len, flags);
}

int WSAAPI newWSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	winsock->DbgPrint("WSASend");
	if(winsock->isAlive)
		for(uint32 i = 0; i <= dwBufferCount; i++)
		{
			sendBuf->type = WSASEND;
			sendBuf->length = lpBuffers[0].len;
			memcpy(sendBuf->getData(), lpBuffers[0].buf, sendBuf->length);
			winsock->sendPacket(sendBuf);
		}

	return winsock->_oldWSASend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
}

int WSAAPI newWSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	int returnLength = winsock->_oldWSARecv(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);

	winsock->DbgPrint("WSARecv");
	if(winsock->isAlive)
		if(returnLength == 0)
		{
			recvBuf->type = WSARECV;
			recvBuf->length = lpBuffers[0].len;
			memcpy(recvBuf->getData(), lpBuffers[0].buf, recvBuf->length);
			winsock->sendPacket(recvBuf);
		}

	return returnLength;
}

int WSAAPI newWSASendTo(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr *lpTo, int iToLen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	winsock->DbgPrint("WSASendTo");
	if(winsock->isAlive)
		for(uint32 i = 0; i <= dwBufferCount; i++)
		{
			sendBuf->type = WSASENDTO;
			sendBuf->length = lpBuffers[0].len;
			memcpy(sendBuf->getData(), lpBuffers[0].buf, sendBuf->length);
			winsock->sendPacket(sendBuf);
		}
	return winsock->_oldWSASendTo(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpTo, iToLen, lpOverlapped, lpCompletionRoutine);
}

int WSAAPI newWSARecvFrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	winsock->DbgPrint("WSARecvFrom");
	int returnLength = winsock->_oldWSARecvFrom(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpFrom, lpFromlen, lpOverlapped, lpCompletionRoutine);

	if(winsock->isAlive)
		if(returnLength == 0)
		{
			recvBuf->type = WSARECVFROM;
			recvBuf->length = lpBuffers[0].len;
			memcpy(recvBuf->getData(), lpBuffers[0].buf, recvBuf->length);
			winsock->sendPacket(recvBuf);
		}

	return returnLength;
}