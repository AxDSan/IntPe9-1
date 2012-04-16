#include "Skeleton.h"
#include <Windows.h>

Skeleton *me;
void Skeleton::DbgPrint(const char* format, ...)
{
	//Reset the buffer
	memset(_dbgPrint, 0x0, 512);

	//Logic
	va_list args;
	va_start(args, format);
	vsprintf_s(_dbgPrint, 512, format, args);
	va_end(args);
	OutputDebugStringA(_dbg);
}

bool Skeleton::sendCommand()
{
	return false;
}

bool Skeleton::sendPacket(MessagePacket *packet)
{
	try
	{
		bool ret = _packetQue->try_send((char*)packet, packet->messagePacketSize(), 0); //instant returns, we can have packet loss but keeping client alive is more important
		return ret;
	}
	catch(...)
	{
		DbgPrint("Exception raised on try send...");
		return false;
	}
}

DWORD WINAPI commandListener(LPVOID lpParam) 
{
	CommandControll *command = (CommandControll*)new uint8[sizeof(CommandControll)];
	uint32 recvdSize, priority;

/*
	message_queue *queue = new message_queue(open_only, "CommandControll");
	while(me->isActive)
	{
		queue->receive(command, MQ_MAX_SIZE, recvdSize, priority);
	}*/
	return 0;
} 

Skeleton::Skeleton()
{
	//Initializing of our DbgPrint buffer
	_dbg = (char*)malloc(521);
	memcpy(_dbg, "[IntPe9] ", 9);
	_dbgPrint = &_dbg[9];
	
	me = this;
	isActive = true;
	_upx = new Upx();
	CreateThread( NULL, 0, commandListener, NULL, 0, NULL);
	
	//Initialize all message_queue's
	try
	{
		int8 queueName[MP_QUEUE_NAME_SIZE];
		sprintf_s(queueName, MP_QUEUE_NAME_SIZE, "%s%i", MP_QUEUE_NAME, GetCurrentProcessId());
		_packetQue = new message_queue(open_or_create, queueName, MP_MAX_NO, MP_MAX_SIZE);
		DbgPrint("Opened queue: %s", queueName);
	}
	catch(...)
	{
		DbgPrint("Was not able to open the queue packet queue");
	}
}

Skeleton::~Skeleton()
{
	me->isActive = false;
	free(_dbgPrint);
}