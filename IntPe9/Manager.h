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


class Manager : public QObject
{
	Q_OBJECT

public:
	Manager(QString path);
	~Manager();

	//Property's
	Core *getCore(QString name);
	Sniffer *getActiveSniffer();

	//Methods
	void readCores(QString path);
	
private:
	//Variables
	QVector<Core*> _cores;
	QVector<Sniffer*> _sniffers;
	Sniffer *_activeSniffer;

public slots:
	void stop();
	void updateSniffers();
	void registerSniffer(Sniffer *sniffer);

signals:
	void activateModel(PacketList *);
	
};

#endif