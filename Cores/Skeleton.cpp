#include "Skeleton.h"
#include <Windows.h>

Skeleton *me;
void Skeleton::DbgPrint(const char* format, ...)
{
	#ifdef _DEBUG
	//Reset the buffer
	memset(_dbgPrint, 0x0, 512);

	//Logic
	va_list args;
	va_start(args, format);
	vsprintf_s(_dbgPrint, 512, format, args);
	va_end(args);
	OutputDebugStringA(_dbg);
	#endif
}

bool Skeleton::sendCommand()
{
	return false;
}

bool Skeleton::sendPacket(MessagePacket *packet)
{
	if(!isAlive)
		return false;
	try
	{
		return _packetQue->try_send((char*)packet, packet->messagePacketSize(), 0); //instant returns, we can have packet loss but keeping client alive is more important
	}
	catch(...)
	{
		DbgPrint("Exception raised on try send...");
		return false;
	}
}

void Skeleton::handleCommand(CommandControll *command)
{
	switch(command->cmd)
	{
		case START:
			DbgPrint("Received START command");
			start();
		break;
		case EXIT:
			DbgPrint("Received EXIT command");
			stop();           //Remove the queue
		break;
		case PYTHON:
			DbgPrint("Received PYTHON command with length: %i", command->length);
			debugToChat(command->getData(), command->length);
		break;
		default:
			DbgPrint("Unknown command");
	}
}

DWORD WINAPI commandListener(LPVOID lpParam) 
{
	CommandControll *command = (CommandControll*)new uint8[CC_MAX_SIZE];
	uint32 recvdSize, priority;

	int8 queueName[CC_QUEUE_NAME_SIZE];
	sprintf_s(queueName, CC_QUEUE_NAME_SIZE, "%s%i", CC_QUEUE_NAME, GetCurrentProcessId());

	message_queue *queue;
	try
	{
		queue = new message_queue(open_or_create, queueName, CC_MAX_NO, CC_MAX_SIZE);
	}
	catch(...)
	{
		me->DbgPrint("Exception raised on opening of command queue");
		return 0;
	}
	
	//Start listener for command
	me->DbgPrint("Waiting for orders, sir!");
	while(me->isRunning)
	{
		if(queue->try_receive(command, CC_MAX_SIZE, recvdSize, priority))
			me->handleCommand(command); //Handle it
		Sleep(100);
	}
	return 0;
} 

bool Skeleton::stop()
{
	isAlive = false;
	Sleep(200);
	delete _packetQue;
	_packetQue = NULL;

	return true;
}

bool Skeleton::start()
{
	if(isAlive) //Is already alive so nothing to do
		return true;

	//Initialize all message_queue's
	int8 queueName[MP_QUEUE_NAME_SIZE];
	sprintf_s(queueName, MP_QUEUE_NAME_SIZE, "%s%i", MP_QUEUE_NAME, GetCurrentProcessId());
	try
	{
		_packetQue = new message_queue(open_or_create, queueName, MP_MAX_NO, MP_MAX_SIZE);
		DbgPrint("Opened queue: %s", queueName);
	}
	catch(...)
	{
		DbgPrint("Was not able to open the queue packet queue");
		return isAlive = false;
	}

	
	return isAlive = true;
}

Skeleton::Skeleton()
{
#ifdef _DEBUG
	_dbg = (char*)malloc(521);
	memcpy(_dbg, "[IntPe9] ", 9);
	_dbgPrint = &_dbg[9];
#endif

	//Set some vars
	isAlive = false;
	isRunning = true;
	me = this;
	_upx = new Upx();

	//Start the packet queue and start command queue thread
	start();
	CreateThread( NULL, 0, commandListener, NULL, 0, NULL);
}

Skeleton::~Skeleton()
{
	isRunning = false;
	isAlive = false;
	stop();
	free(_dbgPrint);
}