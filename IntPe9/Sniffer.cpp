#include "Sniffer.h"
#include <QMessageBox>

Sniffer::Sniffer(Core *core, uint32 pid)
{
	//Set up some variables
	_core = core;
	_pid = pid;

	//Gui NULLing
	packetView = NULL;
	_layout = NULL;
	_view = NULL;

	//Setup this thread
	_thread = new QThread;
	moveToThread(_thread);
	_thread->start();

	//Setup the packet list model + packet listener
	_isStopped = false;
	_packetList = new PacketList(_pid);

	//Start the main thread
	try
	{
		sprintf_s(_controllName, CC_QUEUE_NAME_SIZE, "%s%i", CC_QUEUE_NAME, _pid);
		_controllIpc = new message_queue(open_or_create, _controllName, CC_MAX_NO, CC_MAX_SIZE);
	}
	catch(boost::interprocess::interprocess_exception &ex)
	{
		QString str(ex.what());
	}

	//Create the queue
	recvPacket = (MessagePacket*)new uint8[MP_MAX_SIZE];
	try
	{
		sprintf_s(_packetName, MP_QUEUE_NAME_SIZE, "%s%i", MP_QUEUE_NAME, _pid);
		_packetQueue = new message_queue(open_or_create, _packetName, MP_MAX_NO, MP_MAX_SIZE);

		QMetaObject::invokeMethod(this, "packetPoll", Qt::QueuedConnection);

		//Start the core (or restart if it was already injected)
		sendCommand(START);

	}
	catch(boost::interprocess::interprocess_exception &ex)
	{
		QString str(ex.what());
	}
}

void Sniffer::started()
{

}

void Sniffer::destroy()
{
	blockSignals(true);

	//Stop the Core
	sendCommand(EXIT);

	//Delete the view
	if(packetView != NULL)
		delete packetView;
	if(_layout != NULL)
		delete _layout;
	if(_view != NULL)
		delete _view;

	//Stop the packet thread
	while(mutexLoop.tryLock(1)); //Wait for the poll to finish

	//Destroy the packet queue
	delete _packetQueue;
	delete[] recvPacket;
	message_queue::remove(_packetName);

	//Delete others
	delete _packetList;
	_thread->quit();
	_thread->deleteLater();
}

Sniffer::~Sniffer()
{

}

void Sniffer::sendCommand(CommandType type)
{
	CommandControll cmd(type);
	_controllIpc->send((char*)&cmd, cmd.totalSize(), 0);
}

void Sniffer::sendCommand(CommandControll *command)
{
	_controllIpc->send((char*)command, command->totalSize(), 0);
}

uint32 Sniffer::getPid()
{
	return _pid;
}

void Sniffer::autoScroll(bool state)
{
	if(state)
		connect(_packetList, SIGNAL(layoutChanged()), packetView, SLOT(scrollToBottom()));
	else
		disconnect(_packetList, SIGNAL(layoutChanged()), packetView, SLOT(scrollToBottom()));
}

void Sniffer::buildGui()
{
	//Build the layout
	_view = new QWidget();
	_layout = new QBoxLayout(QBoxLayout::TopToBottom);
	_view->setLayout(_layout);
	packetView = new QTableView(_view);
	_layout->setContentsMargins(0,0,0,0);
	_layout->addWidget(packetView);
	packetView->setModel(_packetList);

	//Set headers
	packetView->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	packetView->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
	packetView->horizontalHeader()->resizeSection(0, 30);
	packetView->horizontalHeader()->resizeSection(1, 45);
	packetView->horizontalHeader()->resizeSection(2, 420);
	packetView->horizontalHeader()->setStretchLastSection(true);
	packetView->horizontalHeader()->setHighlightSections(false);
	packetView->verticalHeader()->setDefaultSectionSize(17);
	packetView->verticalHeader()->setVisible(false);
	packetView->setIconSize(QSize(0, 0));
	packetView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	
	//Set options
	packetView->setAlternatingRowColors(true);
	packetView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	packetView->setSelectionMode(QAbstractItemView::SingleSelection);
	packetView->setDragDropOverwriteMode(false);
	packetView->setDragDropMode(QAbstractItemView::NoDragDrop);
	packetView->setAlternatingRowColors(true);
	packetView->setSelectionBehavior(QAbstractItemView::SelectRows);
	packetView->setWordWrap(false);
	packetView->setCornerButtonEnabled(false);

	autoScroll(true);

	//Set font
	QFont font;
	font.setFamily(QString::fromUtf8("Consolas"));
	packetView->setFont(font);
}

bool Sniffer::savePacketsToFile()
{
	try
	{
		QString path = QFileDialog::getSaveFileName(getView(), tr("Save all packets"), NULL, tr("Packets (*.pacs)"));

		QFile file(path);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			return false;
		}

		QTextStream out(&file);

		for(uint32 i = 0; i < _packetList->rowCount(); i++)
		{
			Packet *packet = _packetList->getPacketAt(i);

			//Format it
			out << packet->strInfoHeader();
			out << packet->strFullDump();
			out << "\n";
		}

		file.close();
	}
	catch(...)
	{
		return false;
	}
	return true;
}

PacketList *Sniffer::getPacketList()
{
	return _packetList;
}

bool Sniffer::isStopped()
{
	return _isStopped;
}

Core *Sniffer::getCore()
{
	return _core;
}

QWidget *Sniffer::getView()
{
	return _view;
}

void Sniffer::packetPoll()
{
	while(_thread->isRunning())
	{
		mutexLoop.lock();
		uint32 recvdSize, priority;

		//Get all packets from the queue
		while(_packetQueue->get_num_msg() > 0)
		{
			if(_packetQueue->try_receive(recvPacket, MP_MAX_SIZE, recvdSize, priority))
				if(recvPacket->messagePacketSize() == recvdSize)
				{
					//Handle this packet
					_packetList->addPacket(new Packet(recvPacket));
				}
		}
		mutexLoop.unlock();
		_thread->wait(20);
	}
	//QMetaObject::invokeMethod(this, "packetPoll", Qt::DirectConnection);
}