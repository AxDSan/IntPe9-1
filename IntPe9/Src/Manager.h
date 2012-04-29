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

#include <Common.h>

#include <QObject>
#include <QDir>
#include <QFile>
#include <QMap>
#include <QList>

#include "Core.h"
#include "Sniffer.h"

#include "Models/SnifferList.h"
#include "Models/CoreList.h"

class Manager : public QObject
{
	Q_OBJECT

public:
	Manager(QString path);
	~Manager();

	//Property's
	Core *getCore(QString name);
	Sniffer *getActiveSniffer();
	Sniffers *getSniffers();
	Cores *getCores();
	SnifferList *getSnifferModel();
	CoreList *getCoreModel();

	//Methods
	void readCores(QString path);
	
private:
	//Variables
	Cores _cores;
	Sniffers _sniffers;
	Sniffer *_activeSniffer;

	//Models
	SnifferList *_snifferList;
	CoreList *_coreList;

public slots:
	void stop();
	void updateSniffers();
	void registerSniffer(Sniffer *sniffer);
	void setActiveSniffer(const QModelIndex &index);

	//Current active sniffer slots
	void activeSaveAll();

signals:
	void activeSnifferChanged(Sniffer *);

};

#endif