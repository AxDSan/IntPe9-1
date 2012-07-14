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
#ifndef INJECTOR_H
#define INJECTOR_H

#include <Common.h>

#include <QDialog>
#include <QSet>
#include <QThread>
#include <QTimer>
#include <QFileIconProvider>
#include "Core.h"
#include "Sniffer.h"
#include "Manager.h"

//Models
#include "Models/CoreList.h"

//Views
#include "ui_ProcessList.h"

typedef QMap<uint32, QString> ProcessList;
/**
 * @class	Injector
 * @brief	A threaded class that manages injection and registration of cores and sniffer instances
 * @author	Intline9
 * @date	18-4-2012
 */
class Injector : public QObject
{
	Q_OBJECT

public:
	Injector(Manager *manager, QWidget *parent = NULL);
	~Injector();

	//Methods
	bool injectAll();
	bool inject(uint32 pid, Core *core);
	bool isInjected(uint32 pid, Core *core);
	ProcessList getProcesses();
	QPixmap getIcon(uint32 pid);
	bool enableDebugPrivilege();
		
	//Constants
	uint32 getTimeout();

public slots:
	void start();
	void stop();
	void eventLoop();
	void refreshProcessList();
	void selectProcess(const QModelIndex &index);
	void selectedProcess(const QModelIndex &index);
	

signals:
	void finished();
	void sniffersChanged();
	
private:
	//Threading
	QTimer *_eventTimer;
	QThread *_thread;
	
	//Variables
	Manager *_manager;
	QSet<uint32> _injected;
	Core *_selectedCore;

	//View
	QWidget *_parent;
	QDialog *_processGui;
	Ui::processView _processView;
};

#endif