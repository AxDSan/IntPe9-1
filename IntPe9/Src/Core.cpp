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
		getCoreInfo(&_info);
		FreeLibrary(library);
		_hasInfo = true;
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