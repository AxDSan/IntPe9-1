#include "Sniffer.h"
#include <Windows.h>

//Constants
uint32 Sniffer::getTimeout()
{
	return 100;
}

Sniffer::Sniffer(uint32 pid, QString processName, Core *core)
{
	//Set up some variables
	_core = core;
	_pid = pid;
	isDead = false;
	_processName = processName;

	//Move to thread
	_thread = new QThread();
	moveToThread(_thread);

	//Setup the connection to manage this thread
	connect(_thread, SIGNAL(started()), this, SLOT(start()));
	connect(this, SIGNAL(finished()), _thread, SLOT(quit()));
	connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
	connect(_thread, SIGNAL(finished()), _thread, SLOT(deleteLater()));

	//Start the thread and let the this->start() handle the real initial work
	_thread->start();
}

Sniffer::~Sniffer()
{
	//Destroy the queue's and buffers
	delete _packetIpc;
	delete _controlIpc;
	delete[] recvPacket;
	delete _packetList;
}

void Sniffer::start()
{
	//Create some stuff
	_packetList = new PacketList();
	recvPacket = (MessagePacket*)new uint8[MP_MAX_SIZE];

	//Open the queue's
	sprintf_s(_controlName, CC_QUEUE_NAME_SIZE, "%s%i", CC_QUEUE_NAME, _pid);
	sprintf_s(_packetName, MP_QUEUE_NAME_SIZE, "%s%i", MP_QUEUE_NAME, _pid);
	_controlIpc = new message_queue(open_or_create, _controlName, CC_MAX_NO, CC_MAX_SIZE);
	_packetIpc = new message_queue(open_or_create, _packetName, MP_MAX_NO, MP_MAX_SIZE);

	//Create event loop
	_eventTimer = new QTimer();
	connect(_eventTimer, SIGNAL(timeout()), this, SLOT(eventLoop()));
	
	//Start the core (or restart if it was already injected)
	sendCommand(START);

	//Start the eventLoop
	_eventTimer->start(getTimeout());

	//Done loading so notify
	emit doneLoading(this);
}

void Sniffer::stop()
{
	_eventTimer->blockSignals(true);  //Stop further processing of the eventLoop
	sendCommand(EXIT);                //Stop the core
	emit finished();                  //Send the stop signal to thread and myself
}

void Sniffer::eventLoop()
{
	if(isDead) return;
	if(_ticks > 50) //Check every 5 seconds
	{
		_ticks = 0;
		if(!isProcessRunning())
		{
			_eventTimer->blockSignals(true);
			isDead = true;
			_packetList->refresh();

			//Delete queue's as no one is going to reopen them (pid based)
			message_queue::remove(_packetName);
			message_queue::remove(_controlName);

			return;
		}
	}

	uint32 recvdSize, priority;
	while(_packetIpc->get_num_msg() > 0)                                                //Get all packets from the queue
		if(_packetIpc->try_receive(recvPacket, MP_MAX_SIZE, recvdSize, priority))   //Try to receive a packet
			if(recvPacket->messagePacketSize() == recvdSize)                    //If it is all good
				_packetList->addPacket(new Packet(recvPacket));             //Create the packet (in my thread) and add it to the model

	_ticks++;
}

/************************************************************************/
/*                           Methods                                    */
/************************************************************************/
void Sniffer::sendCommand(CommandType type)
{
	if(isDead) return;
	CommandControll cmd(type);
	_controlIpc->send((char*)&cmd, cmd.totalSize(), 0);
}

void Sniffer::sendCommand(CommandControll *command)
{
	if(isDead) return;
	_controlIpc->send((char*)command, command->totalSize(), 0);
}

bool Sniffer::isProcessRunning()
{
	DWORD code;
	HANDLE check = OpenProcess(PROCESS_QUERY_INFORMATION, false, _pid);
	GetExitCodeProcess(check, &code);
	CloseHandle(check);

	return (code == STILL_ACTIVE);
}

bool Sniffer::savePacketsToFile()
{
	try
	{
		QString path = QFileDialog::getSaveFileName(NULL, tr("Save all packets"), NULL, tr("Packets (*.pacs)"));

		QFile file(path);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			return false;
		}

		QTextStream out(&file);

		for(uint32 i = 0; i < _packetList->rowCount(); i++)
		{
			Packet *packet = _packetList->getPacketAt(i);

			//Format it
			out << packet->strInfoHeader();
			out << packet->strFullDump();
			out << "\n";
		}

		file.close();
	}
	catch(...)
	{
		return false;
	}
	return true;
}


/************************************************************************/
/*                           Property's                                 */
/************************************************************************/
uint32 Sniffer::getPid()
{
	return _pid;
}

Core *Sniffer::getCore()
{
	return _core;
}

PacketList *Sniffer::getPacketList()
{
	return _packetList;
}

QVariant Sniffer::getField(uint32 index)
{
	switch(index)
	{
		case 0:
			return (isDead) ? QPixmap(":/Common/no.png") : QPixmap(":/Common/ok.png");
		case 1:
			return getPid();
		case 2:
			return _packetList->rowCount();
		case 3:
			return _processName;
	}
	return QVariant();
}