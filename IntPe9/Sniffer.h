#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <common.h>

#include <QMutex>
#include <QThread>
#include <QWidget>
#include <QBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

#include "Models/Core.h"
#include "Models/PacketList.h"

#include <boost/interprocess/ipc/message_queue.hpp>
using namespace boost::interprocess;

class Sniffer : public QObject
{
	Q_OBJECT

public:
	Sniffer(Core *core, uint32 pid);
	~Sniffer();

	//Property's
	QWidget *getView();
	Core *getCore();
	bool isStopped();
	void buildGui();
	PacketList *getPacketList();
	uint32 getPid();
	QTableView *packetView;

	//Methods
	bool savePacketsToFile();

private:
	Core *_core;
	uint32 _pid;
	bool _isStopped;

	//Threading
	QThread *_thread;
	QTimer *_eventLoop;

	//Interface
	QMutex mutexLoop;
	QWidget *_view;
	QBoxLayout *_layout;
	PacketList *_packetList;

	//Controll IPC
	message_queue *_controllIpc;
	int8 _controllName[CC_QUEUE_NAME_SIZE];

	//Packet IPC
	message_queue *_packetQueue;
	MessagePacket *recvPacket;
	int8 _packetName[MP_QUEUE_NAME_SIZE];

public slots:
	void destroy();
	void autoScroll(bool state);
	void packetPoll();
	void sendCommand(CommandType type);
	void sendCommand(CommandControll *command);
	void started();
};

#endif