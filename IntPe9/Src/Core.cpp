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
#include "Core.h"
#include <Windows.h>

Core::Core(const QFileInfo *dll)
{
	_hasInfo = false;
	_isEnabled = true;
	_dll = *dll;
	try
	{
		HINSTANCE library = LoadLibrary(dll->absoluteFilePath().toStdWString().c_str());
		GetCoreInfo getCoreInfo = (GetCoreInfo)GetProcAddress(library, "getCoreInfo");
		InstallProxy installProxy = (InstallProxy)GetProcAddress(library, "installProxy");

		// Get info from the dll
		getCoreInfo(&_info);
		_hasInfo = true;

		// If proxy call the install routine
		if(isProxy())
			installProxy(dll->absoluteFilePath().toStdString().c_str()); //TODO: If false disable this core

		FreeLibrary(library);
	}
	catch(...)
	{
		
	}

}

Core::~Core()
{

}

void Core::addPid(int pid)
{
	if(_pidList.indexOf(pid) < 0)
		_pidList.push_back(pid);
}

void Core::deletePid(int pid)
{
	_pidList.removeAll(pid);
}

bool Core::isEnabled()
{
	return _isEnabled;
}

void Core::setEnabled(bool state)
{
	_isEnabled = state;

	QSettings settings(INI_FILE, QSettings::IniFormat);
	settings.setValue(INI_S_AUTO_INJECT+getName(), state);
}

QString Core::getName()
{
	if(_hasInfo)
		return QString(_info.name);
	else
		return _dll.baseName();
}

QString Core::getModuleName()
{

	return _dll.fileName();
}

bool Core::hasPython()
{
	if(_hasInfo)
		return _info.hasPython;
	else
		return false;
}

bool Core::isProxy()
{
	if(_hasInfo)
		return _info.isProxy;
	else
		return false;
}

QString Core::getVersion()
{
	if(_hasInfo)
		return QString::number(_info.versionNo.major)+"."+QString::number(_info.versionNo.minor);
	else
		return "0";
}

QString Core::getProcessName()
{
	if(_hasInfo)
		return QString(_info.process);
	else
		return _dll.baseName()+".exe";
}

QString Core::getFullPath()
{
	return _dll.absoluteFilePath();
}

QVariant Core::getField(int column)
{
	switch(column)
	{
		case 0:
			if(_info.hasProcess)
				return QPixmap(":/Common/ok.png");
			else
				return QPixmap(":/Common/no.png");
		case 1:
			if(_info.hasPython)
				return QPixmap(":/Common/ok.png");
			else
				return QPixmap(":/Common/no.png");
		case 2:
			return getName();
		case 3:
			return getVersion();
		case 4:
			return getProcessName();
		default:
			return QVariant();
	}
}