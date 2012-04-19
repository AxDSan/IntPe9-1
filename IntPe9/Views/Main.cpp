#include <Windows.h>
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

	//Set the environment
	setEnvironment(true);

	//Create all sub views
	_aboutGui = new AboutGui(this);

	//Create classesses
	_manager = new Manager(QDir::currentPath()+QDir::separator()+"Cores");
	_injector = new Injector(_manager);

	connect(_mainView.tableSniffers, SIGNAL(doubleClicked(const QModelIndex &)), _manager, SLOT(setActiveSniffer(const QModelIndex &)));

	//Set models
	_mainView.tableSniffers->setModel(_manager->getSnifferModel());
	_mainView.tableSniffers->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	_mainView.tableSniffers->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
	_mainView.tableSniffers->horizontalHeader()->resizeSection(0, 25);
	_mainView.tableSniffers->horizontalHeader()->resizeSection(1, 25);
	_mainView.tableSniffers->horizontalHeader()->resizeSection(2, 40);
	_mainView.tableSniffers->horizontalHeader()->resizeSection(3, 50);
	//Build the toolbar
	QIcon scroll(QPixmap(":/Common/Resources/scrollNo.png"));
	scroll.addPixmap(QPixmap(":/Common/Resources/scroll.png"), QIcon::Normal, QIcon::On);
	scrollAction = new QAction(scroll, tr("Set auto scroll"), this);
	scrollAction->setCheckable(true);

	eraseAction = new QAction(QPixmap(":/Common/Resources/eraser.png"), tr("Clear packet list"), this);
	//Add actions to the scrollbar
	_mainView.toolBar->addAction(scrollAction);
	_mainView.toolBar->addAction(eraseAction);

	
	//Setup connections
	//Toolbar
	connect(scrollAction, SIGNAL(triggered(bool)), this, SLOT(autoScroll(bool)));
	connect(eraseAction, SIGNAL(triggered()), this, SLOT(clearList()));
	
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
	setEnvironment(false);
}


void MainGui::setEnvironment(bool state)
{
	QSettings environment("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", QSettings::NativeFormat);
	QStringList paths = environment.value("Path").toString().split(';');

	if(state)
	{
		if(!paths.contains(QDir::currentPath(), Qt::CaseInsensitive))
		{
			paths.append(QDir::toNativeSeparators(QDir::currentPath()));
			environment.setValue("Path", paths.join(";"));
		}
	}
	else
		if(paths.removeAll(QDir::toNativeSeparators(QDir::currentPath())) > 0)
			environment.setValue("Path", paths.join(";"));

	SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, NULL, (LPARAM)L"Environment", SMTO_ABORTIFHUNG, 5000, NULL);
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