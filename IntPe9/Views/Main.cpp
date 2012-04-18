#include <Windows.h>
#include "Main.h"

MainGui::MainGui(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	//As debug i run from compiler so change the path to the output path of all bins
	#ifdef _DEBUG
		QDir::setCurrent("../../bin/VC100_Debug");
	#endif

	//Init variables
	_mainView.setupUi(this);
	_notConnected = true;

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
	
	//Build the toolbar
	QIcon scroll(QPixmap(":/Common/Resources/scrollNo.png"));
	scroll.addPixmap(QPixmap(":/Common/Resources/scroll.png"), QIcon::Normal, QIcon::On);
	scrollAction = new QAction(scroll, "Set auto scroll", this);
	scrollAction->setCheckable(true);
	_mainView.toolBar->addAction(scrollAction);

	
	
	//Setup connections
	//Toolbar
	connect(scrollAction, SIGNAL(triggered(bool)), this, SLOT(autoScroll(bool)));
	//File bar
	connect(_mainView.actionAbout, SIGNAL(triggered()), _aboutGui, SLOT(slotShow()));
	connect(_mainView.actionSavePackets, SIGNAL(triggered()), this, SLOT(saveAllAsText()));
	connect(_mainView.actionClear_packet_list, SIGNAL(triggered()), this, SLOT(clearList()));
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

void MainGui::saveAllAsText()
{
	Sniffer *sniffer = _manager->getActiveSniffer();
	if(sniffer != NULL)
		sniffer->savePacketsToFile();
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

	//Custom GUI setup
	_mainView.tablePackets->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
	_mainView.tablePackets->horizontalHeader()->resizeSection(0, 30);
	_mainView.tablePackets->horizontalHeader()->resizeSection(1, 50);
	_mainView.tablePackets->horizontalHeader()->resizeSection(2, 420);

	if(_notConnected)
		_notConnected = connect(_mainView.tablePackets->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(selectedPacketChanged(const QModelIndex &, const QModelIndex &)));
}

void MainGui::selectedPacketChanged(const QModelIndex &current, const QModelIndex &previous)
{
	PacketList *model = (PacketList*)current.model();
	Packet *packet = model->getPacketAt(current.row());
	_hexView->setData(*packet->getData());
}