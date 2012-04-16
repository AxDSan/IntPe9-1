#include "PacketList.h"
#include <Windows.h>
QMutex mutexList;

PacketList::PacketList(uint32 pid, QObject *parent /* = 0 */) : QAbstractListModel(parent)
{
	_pid = pid;
	_running = true;
	_thread = new QThread;
	moveToThread(_thread);
	_thread->start();


	recvPacket = (MessagePacket*)new uint8[MP_MAX_SIZE];
	sprintf_s(_queueName, MP_QUEUE_NAME_SIZE, "%s%i", MP_QUEUE_NAME, _pid);
	OutputDebugStringA(_queueName);
	_packetQueue = new message_queue(open_or_create, _queueName, MP_MAX_NO, MP_MAX_SIZE);

	QMetaObject::invokeMethod(this, "packetPoll", Qt::QueuedConnection);
}

PacketList::~PacketList()
{
	_running = false;
	_thread->exit();

	delete[] recvPacket;
	delete _packetQueue;
	message_queue::remove(_queueName);
	delete _thread;
}

void PacketList::clear()
{
	mutexList.lock();
	Packet *packet;
	foreach(packet, _packets)
		delete packet;
	_packets.clear();
	mutexList.unlock();
	reset();
	emit layoutChanged();
}

void PacketList::packetPoll()
{
	uint32 recvdSize, priority;

	_packetQueue->receive(recvPacket, MP_MAX_SIZE, recvdSize, priority);
	if(recvPacket->messagePacketSize() != recvdSize)
		throw; //Malformed packet
	else
	{
		//Handle this packet
		mutexList.lock();
		_packets.push_back(new Packet(recvPacket));
		emit layoutChanged();
		mutexList.unlock();
	}
	//packetQueue

	//Loop
	QMetaObject::invokeMethod(this, "packetPoll", Qt::QueuedConnection);	
}

Packet *PacketList::getPacketAt(int index)
{
	if(index < 0 && index >= _packets.count())
		return NULL;

	return _packets.at(index);
}

int PacketList::columnCount(const QModelIndex &parent) const
{
	return 4;
}

int PacketList::rowCount(const QModelIndex &parent) const
{
	return _packets.count();
}

QVariant PacketList::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.row() >= _packets.count() || index.column() > 3 || (role != Qt::DisplayRole && role != Qt::DecorationRole) )
		return QVariant();

	return _packets.at(index.row())->getField(index.column());
}

QVariant PacketList::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
		return QVariant();

	switch(section)
	{
		case 0:
			return QString();
		case 1:
			return QString("Size");
		case 2:
			return QString("Data");
		case 3:
			return QString("Description");
		default:
			return QString();
	}
}