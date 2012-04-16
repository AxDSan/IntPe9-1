#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <common.h>
#include <QThread>
#include <QWidget>
#include <QBoxLayout>
#include <QTableView>
#include <QHeaderView>

#include "Models/Core.h"
#include "Models/PacketList.h"

class Communication : public QThread
{
	Q_OBJECT

public:
	Communication(Core *core, uint32 pid);
	~Communication();

	QWidget *getView();
	Core *getCore();
	bool isStopped();
	void buildGui();

	uint32 getPid();
	QTableView *packetView;

private:
	Core *_core;
	uint32 _pid;
	bool _running, _isStopped;
	QThread *_thread;
	
	//Interface
	QWidget *_view;
	QBoxLayout *_layout;
	PacketList *_packetList;

public slots:
	void mainLoop();

};

#endif