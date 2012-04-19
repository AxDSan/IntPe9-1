#include "Core.h"


Core::Core(const QFileInfo *dll)
{
	_dll = *dll;
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

QString Core::getBaseName()
{
	return _dll.fileName();

}

QString Core::getExeName()
{
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
			return getIcon();
		case 1:
			return _pidList.size();
		case 2:
			return _dll.baseName();
		default:
			return QVariant();
	}
}

QPixmap Core::getIcon()
{
	return QPixmap(":/Common/ok.png");
/*
	if(_type == WSASENDTO)
		return QPixmap(":/Common/Resources/send.gif");
	else if(_type == WSARECVFROM)
		return QPixmap(":/Common/Resources/recv.gif");
	else if(_type == WSARECV)
		return QPixmap(":/Common/Resources/logo.png");
	else*/
		return QPixmap();
}