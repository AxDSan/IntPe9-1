#include "Manager.h"

Manager::Manager(QString path)
{
	readCores(path);
	_activeSniffer = NULL;
}

Manager::~Manager()
{

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
Sniffer *Manager::getActiveSniffer()
{
	return _activeSniffer;
}

Core *Manager::getCore(QString name)
{
	Core *core;
	foreach(core, _cores)
		if(core->getExeName() == name)
			return core;
	return NULL;
}