/*
IntPe9 a open source multi game, general, all purpose and educational packet editor
Copyright (C) 2012  Intline9 <Intline9@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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

	emit activeSnifferChanged(sniffer);
}

void Manager::readCores(QString path)
{
	QDir dir(path);
	QStringList filter("*.dll");
	dir.setNameFilters(filter);
	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
	QFileInfoList list = dir.entryInfoList();

	QSettings settings(INI_FILE, QSettings::IniFormat);
	for (int i = 0; i < list.size(); ++i)
	{
		const QFileInfo *fileInfo = &list.at(i);
		Core *core = new Core(fileInfo);
		core->setEnabled(settings.value(INI_S_AUTO_INJECT+core->getName(), false).toBool());
		_cores.push_back(core);
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
	emit activeSnifferChanged(_activeSniffer);
}
bool Manager::hasSniffer(int32 pid)
{
	Sniffer *sniffer;
	foreach(sniffer, _sniffers)
		if(sniffer->getPid() == pid)
		return true;
	return false;
}

Core *Manager::getCore(QString name, bool enabled)
{
	Core *core;
	foreach(core, _cores)
		if(core->getProcessName() == name && core->isEnabled() == enabled)
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