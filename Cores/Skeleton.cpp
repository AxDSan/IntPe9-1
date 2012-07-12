#include "Skeleton.h"
#include <Windows.h>

Skeleton *me;
void Skeleton::DbgPrint(const char* format, ...)
{
	#ifdef _DEBUG
	//Reset the buffer
	memset(_dbgPrint, 0x0, DBG_SIZE);

	//Logic
	va_list args;
	va_start(args, format);
	vsprintf_s(_dbgPrint, DBG_SIZE, format, args);
	va_end(args);
	OutputDebugStringA(_dbg);
	#endif
}

bool Skeleton::sendCommand()
{
	return false;
}

bool Skeleton::sendMessagePacket(MessagePacket *packet)
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
			parsePython((const char*)command->getData());
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
		Sleep(10);
	}
	me->DbgPrint("Done running command listener!");
	ExitThread(0);
	return 0;
} 

bool Skeleton::stop()
{
	isAlive = false;
	Sleep(200);
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

void Skeleton::getCoreInfo(CoreInfo *info)
{
	memcpy(info->name, name, strlen(name)+1);
	memcpy(info->process, process, strlen(process)+1);
	info->versionNo = versionNo;
	info->hasProcess = hasProcess;
	info->hasPython = hasPython;
}

void Skeleton::startThread()
{
	DbgPrint("Trying to initialize python!");
	//Initialize python
	Py_Initialize();

	//Start the packet queue and start command queue thread
	start();
	CreateThread( NULL, 0, commandListener, NULL, 0, NULL);
}

Skeleton::Skeleton()
{
#ifdef _DEBUG
	_dbg = (char*)malloc(DBG_SIZE+9);
	memcpy(_dbg, "[IntPe9] ", 9);
	_dbgPrint = &_dbg[9];
#endif

	//Set some vars
	isAlive = false;
	isRunning = true;
	me = this;
	_upx = new Upx();
}

void Skeleton::exit()
{
	isRunning = false;
	isAlive = false;
	stop();
}

Skeleton::~Skeleton()
{
	delete _packetQue;
	_packetQue = NULL;
	free(_dbgPrint);
}