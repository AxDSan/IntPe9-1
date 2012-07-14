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
#ifndef MAIN_H
#define MAIN_H

#include <Common.h>

#include <QMessageBox>
#include <QtGui/QMainWindow>
#include <QStyledItemDelegate>
#include <QFile>
#include <QSettings>
#include "QHexEdit/qhexedit.h"

//Ui
#include "ui_Main.h"

//Classes
#include "Manager.h"
#include "Injector.h"

//Extra views/handlers
#include "Parser.h"
#include "About.h"
#include "FilterView.h"

class MainGui : public QMainWindow
{
	Q_OBJECT

public:
	MainGui(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MainGui();

	//Methods
	void setEnvironment(bool state);

	//Property
	Ui::mainView* getView();

public slots:
	void clearList();
	void closing();

	void setActiveSniffer(Sniffer *sniffer);
	void selectedPacketChanged(const QModelIndex &current, const QModelIndex &previous);
	void autoScroll(bool state);
	void installPython();
	void addDebugString(char *str);

	//Copy actions
	void copyAsC();
	void copyAsPython();

	//Menu	
	void ShowHexContextMenu(const QPoint &point);

private:
	bool _firstModel;
	Manager *_manager;
	Injector *_injector;

	//Views
	QHexEdit *_hexView;
	ParserGui *_parserGui;
	AboutGui *_aboutGui;
	FilterView *_filterView;
	Ui::mainView _mainView;
};

#endif // MAIN_H
