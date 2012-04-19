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

#ifndef MANAGER_H
#define MANAGER_H

#include <common.h>

#include <QObject>
#include <QDir>
#include <QFile>
#include <QMap>
#include <QList>

#include "Models/Core.h"
#include "Sniffer.h"

class SnifferList;

class Manager : public QObject
{
	Q_OBJECT

public:
	Manager(QString path);
	~Manager();

	//Property's
	Core *getCore(QString name);
	Sniffer *getActiveSniffer();
	QVector<Sniffer*> *getSniffers();
	QVector<Core*> *getCores();
	SnifferList *getSnifferModel();

	//Methods
	void readCores(QString path);
	
private:
	//Variables
	QVector<Core*> _cores;
	QVector<Sniffer*> _sniffers;
	Sniffer *_activeSniffer;

	//Models
	SnifferList *_snifferList;

public slots:
	void stop();
	void updateSniffers();
	void registerSniffer(Sniffer *sniffer);
	void setActiveSniffer(const QModelIndex &index);

	//Current active sniffer slots
	void activeSaveAll();

signals:
	void activateModel(PacketList *);

};

class SnifferList : public QAbstractListModel
{
	Q_OBJECT

public:
	SnifferList(Manager *manager)
	{
		_manager = manager;
		_header << "S" << "R" << "Pid" << "Packets" << "Process";
	}

	//Standard implementations
	int columnCount(const QModelIndex &parent) const{return _header.count();}
	int rowCount(const QModelIndex &parent = QModelIndex()) const {return _manager->getSniffers()->count();}
	QVariant data(const QModelIndex &index, int role) const
	{
		if (!index.isValid() || index.row() >= _manager->getSniffers()->count() || index.column() > _header.count() || (role != Qt::DisplayRole && role != Qt::DecorationRole) )
			return QVariant();
		if(index.column() == 0)
		{
			if(_manager->getActiveSniffer() == _manager->getSniffers()->at(index.row()))
				return ">";
			else
				return QVariant();
		}
		return _manager->getSniffers()->at(index.row())->getField(index.column()-1);
	}
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
	{
		if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
			return QVariant();
		return _header.at(section);
	}
public slots:
	void refresh() {emit layoutChanged();}


private:
	QStringList _header;
	Manager *_manager;

};

#endif