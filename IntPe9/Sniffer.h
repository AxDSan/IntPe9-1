//IntPe9 an general all purpose packet editor
//Copyright (C) 2012  Intline9 <Intline9@gmail.com>
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef SNIFFER_H
#define SNIFFER_H

#include <common.h>

#include <QThread>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QTimer>

#include "Models/Core.h"
#include "Models/PacketList.h"

#include <boost/interprocess/ipc/message_queue.hpp>
using namespace boost::interprocess;

class Sniffer : public QObject
{
	Q_OBJECT

public:
	Sniffer(uint32 pid, Core *core);
	
	//Property's
	Core *getCore();
	uint32 getPid();
	PacketList *getPacketList();

	//Methods (run from own thread)
	bool savePacketsToFile();
	
	//Constants
	uint32 getTimeout();

private:
	//Let my own eventLoop call me
	~Sniffer();

	//Variables
	bool isDead;
	uint32 _ticks;
	Core *_core;
	uint32 _pid;

	//Threading
	QThread *_thread;
	QTimer *_eventTimer;

	//Model
	PacketList *_packetList;

	//control IPC
	message_queue *_controlIpc;
	int8 _controlName[CC_QUEUE_NAME_SIZE];

	//Packet IPC
	message_queue *_packetIpc;
	MessagePacket *recvPacket;
	int8 _packetName[MP_QUEUE_NAME_SIZE];

	//Methods
	bool isProcessRunning();

public slots:
	void start();
	void stop();
	void eventLoop();
	void sendCommand(CommandType type);
	void sendCommand(CommandControll *command);

signals:
	void finished();
	void doneLoading(Sniffer*);
	
};

#endif