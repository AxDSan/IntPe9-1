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

	//Install my python version (if they dident have it yet)
	installPython();

	//Create all sub views
	_aboutGui = new AboutGui(this);
	_parserGui = new ParserGui(this);

	//Create classesses
	_manager = new Manager(QDir::currentPath()+QDir::separator()+"Cores");
	_injector = new Injector(_manager, this);

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

	//Build the toolbar
	QIcon scroll(QPixmap(":/Common/scrollNo.png"));
	scroll.addPixmap(QPixmap(":/Common/scroll.png"), QIcon::Normal, QIcon::On);
	scrollAction = new QAction(scroll, tr("Set auto scroll"), this);
	scrollAction->setCheckable(true);

	eraseAction = new QAction(QPixmap(":/Common/eraser.png"), tr("Clear packet list"), this);
	pythonAction = new QAction(QPixmap(":/Common/python.png"), tr("Start python interpreter"), this);
	//Add actions to the scrollbar
	_mainView.toolBar->addAction(scrollAction);
	_mainView.toolBar->addAction(eraseAction);
	_mainView.toolBar->addAction(pythonAction);

	
	//Setup connections
	//Toolbar
	connect(scrollAction, SIGNAL(triggered(bool)), this, SLOT(autoScroll(bool)));
	connect(eraseAction, SIGNAL(triggered()), this, SLOT(clearList()));
	connect(pythonAction, SIGNAL(triggered()), this, SLOT(startPython()));
	
	//Action menu
	connect(_mainView.actionAbout, SIGNAL(triggered()), _aboutGui, SLOT(slotShow()));
	connect(_mainView.actionSavePackets, SIGNAL(triggered()), _manager, SLOT(activeSaveAll()));
	connect(_mainView.actionClear_packet_list, SIGNAL(triggered()), this, SLOT(clearList()));
	connect(_mainView.actionShowCores, SIGNAL(triggered()), _mainView.dockCores, SLOT(show()));
	connect(_mainView.actionShowSniffers, SIGNAL(triggered()), _mainView.dockSniffer, SLOT(show()));
	//Others
	connect(_manager, SIGNAL(activateModel(PacketList*)), this, SLOT(setPacketModel(PacketList*)));
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
void MainGui::startPython()
{
	Sniffer *sniffer = _manager->getActiveSniffer();
	if(sniffer != NULL)
	{
		_parserGui->showAndActivate(sniffer);
	}
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
		scrollAction->setChecked(state);
	}
	else
		scrollAction->setChecked(false);
}

void MainGui::setPacketModel(PacketList *model)
{
	_hexView->setData(NULL);
	_mainView.tablePackets->setModel(model);
	autoScroll(true);

	if(_firstModel)
	{
		//Header setup
		_mainView.tablePackets->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
		_mainView.tablePackets->horizontalHeader()->resizeSection(0, 25);
		_mainView.tablePackets->horizontalHeader()->resizeSection(1, 45);
		_mainView.tablePackets->horizontalHeader()->resizeSection(2, 420);

		//Selection model connect
		_firstModel = connect(_mainView.tablePackets->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(selectedPacketChanged(const QModelIndex &, const QModelIndex &)));
	}
}

void MainGui::selectedPacketChanged(const QModelIndex &current, const QModelIndex &previous)
{
	PacketList *model = (PacketList*)current.model();
	Packet *packet = model->getPacketAt(current.row());
	_hexView->setData(*packet->getData());
}