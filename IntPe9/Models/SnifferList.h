/*
IntPe9 a open source multi game, general, all purpose and educational packet editor
Copyright (C) 2012  Intline9 <Intline9@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef SNIFFER_LIST_H
#define SNIFFER_LIST_H

#include <Common.h>

#include <QTableView>
#include <QAbstractListModel>
#include <QVariant>
#include <QPixmap>

#include "Sniffer.h"
typedef QVector<Sniffer*> Sniffers;

class SnifferList : public QAbstractListModel
{
	Q_OBJECT

public:
	SnifferList(Sniffers *sniffers, Sniffer **activeSniffer);

	//Standard implementations
	int columnCount(const QModelIndex &parent) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

public slots:
	void refresh();

private:
	QStringList _header;
	Sniffers *_sniffers;
	Sniffer **_activeSniffer;

};

#endif