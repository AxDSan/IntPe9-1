#ifndef MAIN_H
#define MAIN_H

#include <QMessageBox>
#include <QtGui/QMainWindow>
#include <QStyledItemDelegate>
#include <QFile>

//Ui
#include "ui_Main.h"

//Classes
#include "Models/PacketList.h"
#include "QHexEdit/qhexedit.h"

//Extra views/handlers
#include "About.h"
#include "Cores.h"

class MainGui : public QMainWindow
{
	Q_OBJECT

public:
	MainGui(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MainGui();

	Ui::mainView* getView();

	public slots:
		void saveAllAsText();
		void slotOnClickPacketList(const QModelIndex &current, const QModelIndex &previous);
		void slotShow();
		void slotHide();

		void clearList();

	

private:
	//Views
	QHexEdit *_hexView;
	AboutGui *_aboutGui;
	Ui::mainView _mainView;
	Cores *_cores;
	PacketList *_packetList;
};

#endif // MAIN_H
