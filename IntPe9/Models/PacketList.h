#ifndef PACKET_LIST_H
#define PACKET_LIST_H

#include <QAbstractListModel>
#include <QVector>
#include <QVariant>
#include <QPixmap>
#include <QThread>
#include <QMetaObject>
#include <QMutex>

#include <common.h>
#include "Packet.h"
#include <boost/interprocess/ipc/message_queue.hpp>

using namespace boost::interprocess;

class PacketList : public QAbstractListModel
{
	Q_OBJECT

public:
	PacketList(uint32 pid, QObject *parent = 0);
	~PacketList();

	int columnCount(const QModelIndex &parent) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	Packet *getPacketAt(int index);
	void clear();

private:
	uint32 _pid;
	bool _running;
	QVector<Packet*> _packets;
	QThread *_thread;

	//Packet IPC
	message_queue *_packetQueue;
	MessagePacket *recvPacket;
	int8 _queueName[MP_QUEUE_NAME_SIZE];

//This thread is main loop so it can not recive other signals
public slots:
	void packetPoll();

};

#endif