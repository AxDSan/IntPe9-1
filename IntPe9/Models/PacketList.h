#ifndef PACKET_LIST_H
#define PACKET_LIST_H

#include <QAbstractListModel>
#include <QVector>
#include <QVariant>
#include <QPixmap>
#include <QThread>
#include <QMetaObject>
#include <QMutex>
#include <QTimer>

#include <common.h>
#include "Packet.h"
#include <boost/interprocess/ipc/message_queue.hpp>

using namespace boost::interprocess;

class PacketList : public QAbstractListModel
{
	Q_OBJECT

public:
	PacketList(QObject *parent = 0);
	~PacketList();

	//Reimplements
	int columnCount(const QModelIndex &parent) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	//Custom
	Packet *getPacketAt(int index);
	
	
public slots:
	void clear();
	void addPacket(Packet *packet);

private:
	QThread *_thread;
	QVector<Packet*> _packets;
	QMutex mutexList;

};

#endif