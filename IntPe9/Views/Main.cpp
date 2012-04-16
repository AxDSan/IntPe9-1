#include "Main.h"

MainGui::MainGui(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
#ifdef _DEBUG
	QDir::setCurrent("../../bin/VC100_Debug");
#endif

	_mainView.setupUi(this);

	//Create all sub views
	_aboutGui = new AboutGui(this);

	//Docks
	_cores = new Cores(&_mainView);
	_mainView.tableCores->setModel(_cores);
	_mainView.tableCores->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
	_mainView.tableCores->horizontalHeader()->resizeSection(0, 45);
	_mainView.tableCores->horizontalHeader()->resizeSection(1, 50);
	_mainView.tableCores->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	_cores->readCores("Cores");

	//Other initializing
	_packetList = new PacketList();
	_mainView.packetList->setModel(_packetList);

	_hexView = new QHexEdit(_mainView.widget);
	_hexView->setReadOnly(true);
	_mainView.widget2->layout()->addWidget(_hexView);

	//Custom GUI setup
	_mainView.packetList->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
	_mainView.packetList->horizontalHeader()->resizeSection(0, 30);
	_mainView.packetList->horizontalHeader()->resizeSection(1, 50);
	_mainView.packetList->horizontalHeader()->resizeSection(2, 420);

	connect(_mainView.actionAbout, SIGNAL(triggered()), _aboutGui, SLOT(slotShow()));
	connect(_mainView.actionSavePackets, SIGNAL(triggered()), this, SLOT(saveAllAsText()));
	connect(_mainView.actionClear_packet_list, SIGNAL(triggered()), this, SLOT(clearList()));
	
	connect(_mainView.packetList->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(slotOnClickPacketList(const QModelIndex &, const QModelIndex &)) );
}

MainGui::~MainGui()
{

}

void MainGui::clearList()
{
	_packetList->clear();
	_hexView->setData(NULL);
}

void MainGui::saveAllAsText()
{
	QString path = QFileDialog::getSaveFileName(this, tr("Save all packets"), NULL, tr("Packets (*.pacs)"));

	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		return;
	}

	QTextStream out(&file);

	for(int32 i = 0; i < _packetList->rowCount(); i++)
	{
		Packet *packet = _packetList->getPacketAt(i);

		//Format it
		out << packet->strInfoHeader();
		out << packet->strFullDump();
		out << "\n";
	}

	file.close();
}

void MainGui::slotOnClickPacketList(const QModelIndex &current, const QModelIndex &previous)
{
	Packet *packet = _packetList->getPacketAt(current.row());

	_hexView->setData(*packet->getData());
}

void MainGui::slotShow()
{
	show();
}

void MainGui::slotHide()
{
	hide();
}

Ui::mainView *MainGui::getView()
{
	return &_mainView;
}
