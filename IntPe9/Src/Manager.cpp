#include "Manager.h"

Manager::Manager(QString path)
{
	readCores(path);
	_activeSniffer = NULL;
	_snifferList = new SnifferList(&_sniffers, &_activeSniffer);
	_coreList = new CoreList(&_cores);
}

Manager::~Manager()
{
	delete _snifferList;
	delete _coreList;
}

void Manager::updateSniffers()
{

}

void Manager::stop()
{
	Sniffer *sniffer;
	foreach(sniffer, _sniffers)
		sniffer->stop();
}

void Manager::activeSaveAll()
{
	if(_activeSniffer != NULL)
		_activeSniffer->savePacketsToFile();
}

void Manager::registerSniffer(Sniffer *sniffer)
{
	_activeSniffer = sniffer;
	_sniffers.push_back(sniffer);

	//Connect the models
	connect(sniffer->getPacketList(), SIGNAL(layoutChanged()), _snifferList, SLOT(refresh()));
	_snifferList->refresh();


	emit activateModel(sniffer->getPacketList());
}

void Manager::readCores(QString path)
{
	QDir dir(path);
	QStringList filter("*.dll");
	dir.setNameFilters(filter);
	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
	QFileInfoList list = dir.entryInfoList();

	for (int i = 0; i < list.size(); ++i)
	{
		const QFileInfo *fileInfo = &list.at(i);
		_cores.push_back(new Core(fileInfo));
	}
}

/************************************************************************/
/*                           Property's                                 */
/************************************************************************/
QVector<Sniffer*> *Manager::getSniffers()
{
	return &_sniffers;
}

QVector<Core*> *Manager::getCores()
{
	return &_cores;
}

Sniffer *Manager::getActiveSniffer()
{
	return _activeSniffer;
}

void Manager::setActiveSniffer(const QModelIndex &index)
{
	_activeSniffer = _sniffers.at(index.row());
	_snifferList->refresh();
	emit activateModel(_activeSniffer->getPacketList());
}

Core *Manager::getCore(QString name)
{
	Core *core;
	foreach(core, _cores)
		if(core->getExeName() == name)
			return core;
	return NULL;
}

SnifferList *Manager::getSnifferModel()
{
	return _snifferList;
}

CoreList *Manager::getCoreModel()
{
	return _coreList;
}