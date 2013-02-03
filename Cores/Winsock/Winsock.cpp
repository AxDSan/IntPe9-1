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

uint32 isRecvFrom = 0;
uint32 addressSend = 0;
uint32 addressSendTo = 0;
uint32 addressRecv = 0;
uint32 addressRecvFrom = 0;
uint32 addressSend2 = 0;
uint32 addressSendTo2 = 0;
uint32 addressRecv2 = 0;
uint32 addressRecvFrom2 = 0;
uint32 addressWSASend = 0;
uint32 addressWSARecv = 0;
uint32 addressWSASendTo = 0;
uint32 addressWSARecvFrom = 0;

MessagePacket *sendBuf;
MessagePacket *recvBuf;

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
	
	// Try to hook all send/recv functions from ws2_32 by IAT
	_oldSend2 = (defSend)_upx->hookIatFunction("ws2_32", "send", (unsigned long)&newSend);
	_oldRecv2 = (defRecv)_upx->hookIatFunction("ws2_32", "recv", (unsigned long)&newRecv);
	_oldSendTo2 = (defSendTo)_upx->hookIatFunction("ws2_32", "sendto", (unsigned long)&newSendTo);
	_oldRecvFrom2 = (defRecvFrom)_upx->hookIatFunction("ws2_32", "recvfrom", (unsigned long)&newRecvFrom);
	_oldWSASend = (defWSASend)_upx->hookIatFunction("ws2_32", "WSASend", (unsigned long)&newWSASend);
	_oldWSARecv = (defWSARecv)_upx->hookIatFunction("ws2_32", "WSARecv", (unsigned long)&newWSARecv);
	_oldWSASendTo = (defWSASendTo)_upx->hookIatFunction("ws2_32", "WSASendTo", (unsigned long)&newWSASendTo);
	_oldWSARecvFrom = (defWSARecvFrom)_upx->hookIatFunction("ws2_32", "WSARecvFrom", (unsigned long)&newWSARecvFrom);

	// Try to hook all send/recv function from wsock32 by IAT
	_oldSend = (defSend)_upx->hookIatFunction("wsock32", "send", (unsigned long)&newSend);
	_oldRecv = (defRecv)_upx->hookIatFunction("wsock32", "recv", (unsigned long)&newRecv);
	_oldSendTo = (defSendTo)_upx->hookIatFunction("wsock32", "sendto", (unsigned long)&newSendTo);
	_oldRecvFrom = (defRecvFrom)_upx->hookIatFunction("wsock32", "recvfrom", (unsigned long)&newRecvFrom);

	// Now check for every function if we succeeded and if not lets do some inline hooking
	try
	{
		HMODULE ws2_32 = GetModuleHandle("ws2_32");
		HMODULE wsock32 = GetModuleHandle("wsock32");

		/*if(!_oldSend)
		{
			uint32 address = (uint32)GetProcAddress(wsock32, "WSOCK32.send");
			addressSend = address + OFFSET_JMP;
			DbgPrint("Hooking send by inline: %08X", address);
			if(address)
				Memory::writeJump((uint8*)address, (uint8*)CaveSend);
			else
				DbgPrint("   could not find the function");
		}
		if(!_oldRecv)
		{
			uint32 address = (uint32)GetProcAddress(wsock32, "recv");
			addressRecv = address + OFFSET_RECV;
			DbgPrint("Hooking recv by inline: %08X", addressRecv);
			if(address)
				Memory::writeJump((uint8*)addressRecv, (uint8*)CaveRecv, 1);
			else
				DbgPrint("   could not find the function");
		}
		if(!_oldSendTo)
		{
			uint32 address = (uint32)GetProcAddress(wsock32, "sendto");
			addressSendTo = address + OFFSET_JMP;
			DbgPrint("Hooking sendto by inline: %08X", address);
			if(address)
				Memory::writeJump((uint8*)address, (uint8*)CaveSendTo);
			else
				DbgPrint("   could not find the function");
		}*/
		if(!_oldRecvFrom)
		{
			uint32 address = (uint32)GetProcAddress(wsock32, "recvfrom");
			addressRecvFrom = address + OFFSET_RECVFROM;
			DbgPrint("Hooking recvfrom by inline: %08X", addressRecvFrom);
			if(address)
				Memory::writeJump((uint8*)addressRecvFrom, (uint8*)CaveRecvFrom, 2);
			else
				DbgPrint("   could not find the function");
		}
		
		if(!_oldSend2)
		{
			uint32 address = (uint32)GetProcAddress(ws2_32, "send");
			addressSend2 = address + OFFSET_JMP;
			DbgPrint("Hooking send2 by inline: %08X", address);
			if(address)
				Memory::writeJump((uint8*)address, (uint8*)CaveSend2);
			else
				DbgPrint("   could not find the function");
		}
		if(!_oldRecv2)
		{
			uint32 address = (uint32)GetProcAddress(ws2_32, "recv");
			addressRecv2 = address + OFFSET_RECV2;
			DbgPrint("Hooking recv2 by inline: %08X", addressRecv2);
			if(address)
				Memory::writeJump((uint8*)addressRecv2, (uint8*)CaveRecv2, 1);
			else
				DbgPrint("   could not find the function");
		}
		if(!_oldSendTo2)
		{
			uint32 address = (uint32)GetProcAddress(ws2_32, "sendto");
			addressSendTo2 = address + OFFSET_JMP;
			DbgPrint("Hooking sendto2 by inline: %08X", address);
			if(address)
				Memory::writeJump((uint8*)address, (uint8*)CaveSendTo2);
			else
				DbgPrint("   could not find the function");
		}
		if(!_oldRecvFrom2)
		{
			uint32 address = (uint32)GetProcAddress(ws2_32, "recvfrom");
			addressRecvFrom2 = address + OFFSET_RECVFROM2;
			DbgPrint("Hooking recvfrom2 by inline: %08X", addressRecvFrom2);
			if(address)
				Memory::writeJump((uint8*)addressRecvFrom2, (uint8*)CaveRecvFrom2, 1);
			else
				DbgPrint("   could not find the function");
		}
		if(!_oldWSASend)
		{
			uint32 address = (uint32)GetProcAddress(ws2_32, "WSASend");
			addressWSASend = address + OFFSET_JMP;
			DbgPrint("Hooking WSASend by inline: %08X", address);
			if(address)
				Memory::writeJump((uint8*)address, (uint8*)CaveWSASend);
			else
				DbgPrint("   could not find the function");
		}
		if(!_oldWSARecv)
		{
			uint32 address = (uint32)GetProcAddress(ws2_32, "WSARecv");
			addressWSARecv = address + OFFSET_WSARECV;
			DbgPrint("Hooking WSARecv inline: %08X", addressWSARecv);
			if(address)
				Memory::writeJump((uint8*)addressWSARecv, (uint8*)CaveWSARecv);
			else
				DbgPrint("   could not find the function");
		}
		if(!_oldWSASendTo)
		{
			uint32 address = (uint32)GetProcAddress(ws2_32, "WSASendTo");
			addressWSASendTo = address + OFFSET_JMP;
			DbgPrint("Hooking WSASendTo inline: %08X", address);
			if(address)
				Memory::writeJump((uint8*)address, (uint8*)CaveWSASendTo);
			else
				DbgPrint("   could not find the function");
		}
		if(!_oldWSARecvFrom)
		{
			uint32 address = (uint32)GetProcAddress(ws2_32, "WSARecvFrom");
			addressWSARecvFrom = address + OFFSET_WSARECVFROM;
			DbgPrint("Hooking WSARecvFrom inline: %08X", addressWSARecvFrom);
			if(address)
				Memory::writeJump((uint8*)addressWSARecvFrom, (uint8*)CaveWSARecvFrom);
			else
				DbgPrint("   could not find the function");
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

	delete[]sendBuf;
	delete[]recvBuf;

	exit();
}

// Hooks
int WSAAPI newSend(SOCKET s, const char *buf, int len, int flags)
{
	// Get data and send it to front end
	if(MP_MAX_SIZE < len)
		winsock->DbgPrint("Tried to sniff a packet that is crazy big: %i, and i only have space for: %i", len, MP_MAX_SIZE);
	else
	{
		sendBuf->reset();
		sendBuf->type = SEND;
		sendBuf->length = len;
		memcpy(sendBuf->getData(), buf, sendBuf->length);
		winsock->sendMessagePacket(sendBuf);
	}

	if(addressSend) //Inline hook so let codecave jump back
		return 0;
	return winsock->_oldSend(s, buf, len, flags);
}

int WSAAPI newRecv(SOCKET s, char *buf, int len, int flags)
{
	int bytesRecved = winsock->_oldRecv(s, buf, len, flags);
	inlineRecv2(s, buf, len, flags, bytesRecved);
	return bytesRecved;
}

void WSAAPI inlineRecv2(SOCKET s, char *buf, int len, int flags, int bytesRecved)
{
	if(bytesRecved <= 0)
		return;

	// Get data and send it to front end
	if(MP_MAX_SIZE < bytesRecved)
		winsock->DbgPrint("Tried to sniff a packet that is crazy big: %i, and i only have space for: %i", bytesRecved, MP_MAX_SIZE);
	else
	{
		recvBuf->reset();
		recvBuf->type = RECV;
		recvBuf->length = bytesRecved;
		memcpy(recvBuf->getData(), buf, recvBuf->length);
		winsock->sendMessagePacket(recvBuf);
	}
}

int WSAAPI newSendTo(SOCKET s, const char *buf, int len, int flags, const struct sockaddr *to, int tolen)
{
	// Get data and send it to front end
	if(MP_MAX_SIZE < len)
		winsock->DbgPrint("Tried to sniff a packet that is crazy big: %i, and i only have space for: %i", len, MP_MAX_SIZE);
	else
	{
		sendBuf->reset();
		sendBuf->type = SENDTO;
		sendBuf->length = len;
		memcpy(sendBuf->getData(), buf, sendBuf->length);
		winsock->sendMessagePacket(sendBuf);
	}

	if(addressSend) //Inline hook so let codecave jump back
		return 0;
	return winsock->_oldSendTo(s, buf, len, flags, to, tolen);
}

int WSAAPI newRecvFrom(SOCKET s, char *buf, int len, int flags, struct sockaddr *from, int *fromlen)
{
	int bytesRecved = winsock->_oldRecvFrom(s, buf, len, flags, from, fromlen);
	inlineRecvFrom2(s, buf, len, flags, from, fromlen, bytesRecved);
	return bytesRecved;
}

void WSAAPI inlineRecvFrom2(SOCKET s, char *buf, int len, int flags, struct sockaddr *from, int *fromlen, int bytesRecved)
{
	if(bytesRecved <= 0)
		return;

	// Get data and send it to front end
	if(MP_MAX_SIZE < bytesRecved)
		winsock->DbgPrint("Tried to sniff a packet that is crazy big: %i, and i only have space for: %i", bytesRecved, MP_MAX_SIZE);
	else
	{
		recvBuf->reset();
		recvBuf->type = RECVFROM;
		recvBuf->length = bytesRecved;
		memcpy(recvBuf->getData(), buf, recvBuf->length);
		winsock->sendMessagePacket(recvBuf);
	}
}

int WSAAPI newWSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	for(uint32 i = 0; i <= dwBufferCount; i++)
	{
		if(MP_MAX_SIZE < lpBuffers[i].len)
			winsock->DbgPrint("Tried to sniff a packet that is crazy big: %i, and i only have space for: %i", lpBuffers[i].len, MP_MAX_SIZE);
		else
		{
			sendBuf->reset();
			sendBuf->type = WSASEND;
			sendBuf->length = lpBuffers[i].len;
			memcpy(sendBuf->getData(), lpBuffers[i].buf, sendBuf->length);
			winsock->sendMessagePacket(sendBuf);
		}
	}

	if(addressWSASend) //Inline hook so let codecave jump back
		return 0;
	return winsock->_oldWSASend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
}

int WSAAPI newWSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	int errorNo = 0;
	
	if(!addressWSARecv)
		errorNo = winsock->_oldWSARecv(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);

	if(MP_MAX_SIZE < *lpNumberOfBytesRecvd)
	{
		winsock->DbgPrint("Tried to sniff a packet that is crazy big: %i, and i only have space for: %i", *lpNumberOfBytesRecvd, MP_MAX_SIZE);
		return errorNo;
	}

	if(errorNo == 0 && *lpNumberOfBytesRecvd > 0)
	{
		recvBuf->reset();
		recvBuf->type = WSARECV;
		recvBuf->length = *lpNumberOfBytesRecvd;
		memcpy(recvBuf->getData(), lpBuffers[0].buf, recvBuf->length);
		winsock->sendMessagePacket(recvBuf);
	}

	return errorNo;
}

int WSAAPI newWSASendTo(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr *lpTo, int iToLen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	for(uint32 i = 0; i <= dwBufferCount; i++)
	{
		if(MP_MAX_SIZE < lpBuffers[i].len)
			winsock->DbgPrint("Tried to sniff a packet that is crazy big: %i, and i only have space for: %i", lpBuffers[i].len, MP_MAX_SIZE);
		else
		{
			sendBuf->reset();
			sendBuf->type = WSASENDTO;
			sendBuf->length = lpBuffers[i].len;
			memcpy(sendBuf->getData(), lpBuffers[i].buf, sendBuf->length);
			winsock->sendMessagePacket(sendBuf);
		}
	}
	if(addressWSASendTo) //Inline hook so let codecave jump back
		return 0;
	return winsock->_oldWSASendTo(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpTo, iToLen, lpOverlapped, lpCompletionRoutine);
}

int WSAAPI newWSARecvFrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	int errorNo = 0;

	// Check for inline hook from ws2_32
	if(!addressWSARecvFrom)
	{
		if(isRecvFrom) // Check for inline hook from wsock32
			isRecvFrom = 0;
		else
			errorNo = winsock->_oldWSARecvFrom(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpFrom, lpFromlen, lpOverlapped, lpCompletionRoutine);
	}

	if(MP_MAX_SIZE < *lpNumberOfBytesRecvd)
	{
		winsock->DbgPrint("Tried to sniff a packet that is crazy big: %i, and i only have space for: %i", *lpNumberOfBytesRecvd, MP_MAX_SIZE);
		return errorNo;
	}

	if(errorNo == 0 && *lpNumberOfBytesRecvd > 0)
	{
			recvBuf->reset();
			recvBuf->type = WSARECVFROM;
			recvBuf->length = *lpNumberOfBytesRecvd;
			memcpy(recvBuf->getData(), lpBuffers[0].buf, recvBuf->length);
			winsock->sendMessagePacket(recvBuf);
	}

	return errorNo;
}