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

#ifndef CORE_LIST_H
#define CORE_LIST_H

#include <Common.h>

#include <QTableView>
#include <QAbstractListModel>
#include <QVector>
#include <QVariant>
#include <QPixmap>
#include <QMutex>

#include "Core.h"
typedef QVector<Core*> Cores;

class CoreList : public QAbstractListModel
{
	Q_OBJECT

public:
	CoreList(Cores *cores);

	//Standard implementations
	int columnCount(const QModelIndex &parent) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

public slots:
	void refresh();

private:
	QStringList _header;
	Cores *_cores;

};


#endif