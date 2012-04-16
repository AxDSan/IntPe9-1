#include "Communication.h"

Communication::Communication(Core *core, uint32 pid)
{
	_core = core;
	_pid = pid;
	_running = true;

	packetView = NULL;
	_layout = NULL;
	_view = NULL;

	//Setup this thread
	_thread = new QThread;
	moveToThread(_thread);
	_thread->start();

	//Setup the packet list model + packet listener
	_packetList = new PacketList(_pid);
	
	_isStopped = false;

	//Start the main thread
	QMetaObject::invokeMethod(this, "mainLoop", Qt::QueuedConnection);
}

Communication::~Communication()
{
	_running = false;
	_thread->exit();

	//Delete the view
	if(_view != NULL)
		delete _view;
	if(_layout != NULL)
		delete _layout;
	if(packetView != NULL)
		delete packetView;

	delete _packetList;
	delete _thread;
}

uint32 Communication::getPid()
{
	return _pid;
}

void Communication::buildGui()
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

	//Set font
	QFont font;
	font.setFamily(QString::fromUtf8("Consolas"));
	packetView->setFont(font);
}

bool Communication::isStopped()
{
	return _isStopped;
}

Core *Communication::getCore()
{
	return _core;
}

QWidget *Communication::getView()
{
	return _view;
}

void Communication::mainLoop()
{
	wait(20);
	QMetaObject::invokeMethod(this, "mainLoop", Qt::QueuedConnection);
}