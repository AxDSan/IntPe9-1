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
#ifndef CORE_H
#define CORE_H

#include <Common.h>

#include <QSettings>
#include <QVariant>
#include <QAbstractListModel>
#include <QPixmap>
#include <QFileInfo>
#include <QList>

class Core : public QObject
{
	Q_OBJECT

public:
	Core(const QFileInfo *dll);
	~Core();

	//Custom stuff
	QString getFullPath();
	QVariant getField(int column);

	QString getName();
	QString getVersion();
	QString getModuleName();
	QString getProcessName();

	bool hasPython();
	bool isProxy();
	bool isEnabled();
	void setEnabled(bool state);

	void addPid(int pid);
	void deletePid(int pid);


private:
	QFileInfo _dll;
	QList<int> _pidList;
	bool _hasInfo;
	bool _isEnabled;
	CoreInfo _info;

};

#endif