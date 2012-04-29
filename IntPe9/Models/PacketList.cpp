#include "PacketList.h"

PacketList::PacketList(QObject *parent /* = 0 */) : QAbstractListModel(parent)
{

}

PacketList::~PacketList()
{
	clear();
}

void PacketList::clear()
{
	mutexList.lock();
	Packet *packet;
	foreach(packet, _packets)
		delete packet;
	_packets.clear();
	_filteredPackets.clear();
	mutexList.unlock();
	reset();
	refresh();
}

void PacketList::addPacket(Packet *packet, bool showInTable)
{
	mutexList.lock();
	_packets.push_back(packet);

	if(showInTable)
	{
		_filteredPackets.push_back(packet);
		refresh();
	}

	mutexList.unlock();
}

void PacketList::refresh()
{
	emit layoutChanged();
}

void PacketList::autoScroll(bool state, QTableView *view)
{
	if(state)
		connect(this, SIGNAL(layoutChanged()), view, SLOT(scrollToBottom()));
	else
		disconnect(this, SIGNAL(layoutChanged()), view, SLOT(scrollToBottom()));
}

Packet *PacketList::getPacketAt(int index)
{
	if(index < 0 && index >= _packets.count())
		return NULL;

	return _packets.at(index);
}

QVector<Packet*> *PacketList::getPacketList()
{
	return &_packets;
}

void PacketList::setFilteredList(QVector<Packet*> *packets)
{
	mutexList.lock();
	_filteredPackets = QVector<Packet*>(*packets); //copy
	mutexList.unlock();
	refresh();

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