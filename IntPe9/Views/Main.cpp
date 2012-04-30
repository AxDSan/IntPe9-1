#include <Windows.h>
#include <Shlobj.h>
#include "Main.h"


MainGui::MainGui(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	//In debug we can run from VS that will change the run dir, so fix that! (if needed)
	#ifdef _DEBUG
		QDir corePath("Cores");
		if(!corePath.exists())
			QDir::setCurrent("../../bin/VC100_Debug");
	#endif

	//Init variables
	_mainView.setupUi(this);
	_firstModel = true;

	//Create the hex viewer
	_hexView = new QHexEdit(_mainView.hexWidget);
	_hexView->setReadOnly(true);
	_mainView.hexWidget->layout()->addWidget(_hexView);
	_hexView->setContextMenuPolicy(Qt::CustomContextMenu);

	//Install my python version (if they did not have it yet)
	installPython();

	//Create all sub views
	_aboutGui = new AboutGui(this);
	_parserGui = new ParserGui(this);
	_filterView = new FilterView(this);

	//Create classes
	_manager = new Manager(QDir::currentPath()+QDir::separator()+"Cores");
	_injector = new Injector(_manager, this);

	connect(_mainView.tableFilters, SIGNAL(doubleClicked(const QModelIndex &)), _filterView, SLOT(editFilter(const QModelIndex &)));
	connect(_mainView.tableSniffers, SIGNAL(doubleClicked(const QModelIndex &)), _manager, SLOT(setActiveSniffer(const QModelIndex &)));
	connect(_mainView.tableCores, SIGNAL(doubleClicked(const QModelIndex &)), _injector, SLOT(selectProcess(const QModelIndex &)), Qt::DirectConnection);

	//Set models
	_mainView.tableSniffers->setModel(_manager->getSnifferModel());
	_mainView.tableSniffers->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
	_mainView.tableSniffers->horizontalHeader()->resizeSection(0, 25);
	_mainView.tableSniffers->horizontalHeader()->resizeSection(1, 25);
	_mainView.tableSniffers->horizontalHeader()->resizeSection(2, 40);
	_mainView.tableSniffers->horizontalHeader()->resizeSection(3, 50);
	_mainView.tableCores->setModel(_manager->getCoreModel());
	_mainView.tableCores->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

	//Add actions to the scrollbar
	_mainView.toolBar->addAction(_mainView.scrollAction);
	_mainView.toolBar->addAction(_mainView.eraseAction);
	_mainView.toolBar->addAction(_mainView.pythonAction);
	
	//Setup connections
	//Menu
	connect(_hexView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowHexContextMenu(const QPoint&)));
	connect(_mainView.copyPython, SIGNAL(triggered()), this, SLOT(copyAsPython()));
	connect(_mainView.copyC, SIGNAL(triggered()), this, SLOT(copyAsC()));

	//Toolbar
	connect(_mainView.scrollAction, SIGNAL(triggered(bool)), this, SLOT(autoScroll(bool)));
	connect(_mainView.eraseAction, SIGNAL(triggered()), this, SLOT(clearList()));
	connect(_mainView.pythonAction, SIGNAL(triggered()), _parserGui, SLOT(show()));
	
	//Action menu
	connect(_mainView.actionAbout, SIGNAL(triggered()), _aboutGui, SLOT(slotShow()));
	connect(_mainView.actionSavePackets, SIGNAL(triggered()), _manager, SLOT(activeSaveAll()));
	connect(_mainView.actionClear_packet_list, SIGNAL(triggered()), this, SLOT(clearList()));
	connect(_mainView.actionShowCoresSniffers, SIGNAL(triggered()), _mainView.dockCores, SLOT(show()));
	connect(_mainView.actionShowFilters, SIGNAL(triggered()), _mainView.dockFilters, SLOT(show()));

	//Others
	connect(_mainView.buttonNewFilter, SIGNAL(clicked()), _filterView, SLOT(showEmpty()));
	connect(_manager, SIGNAL(activeSnifferChanged(Sniffer*)), _filterView, SLOT(setSniffer(Sniffer*)));
	connect(_manager, SIGNAL(activeSnifferChanged(Sniffer*)), _parserGui, SLOT(setSniffer(Sniffer*)));
	connect(_manager, SIGNAL(activeSnifferChanged(Sniffer*)), this, SLOT(setActiveSniffer(Sniffer*)));
}

MainGui::~MainGui()
{
	//Disable environment
}

void MainGui::installPython()
{
	wchar_t system32[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, SHGFP_TYPE_DEFAULT, system32);
	QString pythonE9 = QString::fromWCharArray(system32)+QDir::separator()+"PythonE9.dll";
	CopyFile(L"PythonE9.dll", pythonE9.toStdWString().c_str(), TRUE);
}

void MainGui::copyAsC()
{
	Sniffer *sniffer = _manager->getActiveSniffer();
	if(sniffer == NULL)
		return;

	Packet *packet = sniffer->getPacketList()->getPacketAt(_mainView.tablePackets->currentIndex().row());
	if(packet != NULL)
	{
		QClipboard *cb = QApplication::clipboard();
		cb->setText(packet->toC());
	}
}

void MainGui::copyAsPython()
{
	Sniffer *sniffer = _manager->getActiveSniffer();
	if(sniffer == NULL)
		return;

	Packet *packet = sniffer->getPacketList()->getPacketAt(_mainView.tablePackets->currentIndex().row());
	if(packet != NULL)
	{
		QClipboard *cb = QApplication::clipboard();
		cb->setText(packet->toPython());
	}
}

void MainGui::ShowHexContextMenu(const QPoint &point)
{
	QPoint globalPos = _hexView->mapToGlobal(point);

	QMenu menu;
	menu.addAction(_mainView.copyPython);
	menu.addAction(_mainView.copyC);
	menu.exec(globalPos);
}

void MainGui::clearList()
{
	Sniffer *sniffer = _manager->getActiveSniffer();
	if(sniffer != NULL)
	{
		sniffer->getPacketList()->clear();
		_hexView->setData(NULL);
	}
}

void MainGui::closing()
{
	_injector->stop();
	_manager->stop();
}

void MainGui::autoScroll(bool state)
{
	Sniffer *sniffer = _manager->getActiveSniffer();
	if(sniffer != NULL)
	{
		sniffer->getPacketList()->autoScroll(state, _mainView.tablePackets);
		_mainView.scrollAction->setChecked(state);
	}
	else
		_mainView.scrollAction->setChecked(false);
}

void MainGui::setActiveSniffer(Sniffer *sniffer)
{
	_hexView->setData(NULL);
	_mainView.tableFilters->setModel(sniffer->getFilterList());
	_mainView.tablePackets->setModel(sniffer->getPacketList());
	autoScroll(true);

	if(_firstModel)
	{
		//Enable filter button
		_mainView.buttonNewFilter->setEnabled(true);

		//Header setup
		_mainView.tablePackets->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
		_mainView.tablePackets->horizontalHeader()->resizeSection(0, 25);
		_mainView.tablePackets->horizontalHeader()->resizeSection(1, 45);
		_mainView.tablePackets->horizontalHeader()->resizeSection(2, 420);
		_mainView.tableFilters->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
		_mainView.tableFilters->horizontalHeader()->resizeSection(0, 19);
		_mainView.tableFilters->horizontalHeader()->resizeSection(1, 80);
		_mainView.tableFilters->horizontalHeader()->resizeSection(2, 63);

		//Selection model connect
		connect(_mainView.isDefaultHidden, SIGNAL(toggled (bool)), sniffer->getFilterList(), SLOT(setDefaultHide(bool)));
		_firstModel = connect(_mainView.tablePackets->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(selectedPacketChanged(const QModelIndex &, const QModelIndex &)));
	}
}

void MainGui::selectedPacketChanged(const QModelIndex &current, const QModelIndex &previous)
{
	PacketList *model = (PacketList*)current.model();
	Packet *packet = model->getPacketAt(current.row());
	_hexView->setData(*packet->getData());
}