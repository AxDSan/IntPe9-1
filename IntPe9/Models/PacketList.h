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

#ifndef PACKET_LIST_H
#define PACKET_LIST_H

#include <Common.h>

#include <QTableView>
#include <QAbstractListModel>
#include <QVector>
#include <QVariant>
#include <QPixmap>
#include <QMutex>

#include "Packet.h"

class PacketList : public QAbstractListModel
{
	Q_OBJECT

public:
	PacketList(QObject *parent = 0);
	~PacketList();

	//Standard implementations
	int columnCount(const QModelIndex &parent) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	//Methods
	Packet *getPacketAt(int index);
	QVector<Packet*> *getPacketList();
	
	void autoScroll(bool state, QTableView *view);
	
public slots:
	void clear();
	void addPacket(Packet *packet, bool showInTable = true);
	void setFilteredList(QVector<Packet*> *packets);
	void refresh();

private:
	QVector<Packet*> _packets;
	QVector<Packet*> _filteredPackets;
	QMutex mutexList;

};

#endif