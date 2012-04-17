#include "Cores.h"
#include "Views/Main.h"
#include <windows.h>
#include <tlhelp32.h>

Cores::Cores(Ui::mainView *view, QObject *parent /* = 0 */) : QAbstractListModel(parent)
{
	_view = view;
	_running = true;

	_thread = new QThread;
	moveToThread(_thread);
	_thread->start();

	QMetaObject::invokeMethod(this, "listenerLoop", Qt::QueuedConnection);
}

Cores::~Cores()
{
	_running = false;
	_thread->exit();
	delete _thread;
}

void Cores::readCores(QString path)
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
	emit layoutChanged();
}

void Cores::listenerLoop()
{
	bool injected = false;
	while(_running)
	{
		injectAllCores();
		//cleanInjectedList();
		Sleep(50);
	}
}

void Cores::cleanInjectedList()
{
	InjectMap::iterator it = _injected.begin();
	HANDLE check;
	while (it != _injected.end())
	{
		DWORD code;
		check = OpenProcess(PROCESS_QUERY_INFORMATION, false, it.key());
		GetExitCodeProcess(check, &code);
		if(code != STILL_ACTIVE)
		{
			it.value()->deletePid(it.key());
			it = _injected.erase(it);
			emit layoutChanged();
		}
		else
			++it;
		CloseHandle(check);
	}

}

bool Cores::hasCommunication(uint32 pid)
{
	Sniffer *communication;
	foreach(communication, _communications)
		if(communication->getPid() == pid)
			return true;
	return false;
}

Core *Cores::haveCore(QString name)
{
	Core *core;
	foreach(core, _cores)
		if(core->getExeName() == name)
			return core;
	return NULL;
}

bool Cores::isInjected(uint32 pid, Core *core)
{
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;

	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	if(hModuleSnap == INVALID_HANDLE_VALUE)
		return false;

	me32.dwSize = sizeof(MODULEENTRY32);

	if(!Module32First(hModuleSnap, &me32))
	{
		CloseHandle(hModuleSnap);
		return false;
	}

	do
	{
		if(core->getBaseName() == QString::fromWCharArray(me32.szModule))
			return true;
	} while(Module32Next(hModuleSnap, &me32));

	CloseHandle(hModuleSnap);
	return false;
}
bool Cores::injectAllCores()
{
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;

	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if(hProcessSnap == INVALID_HANDLE_VALUE)
		return false;

	pe32.dwSize = sizeof(PROCESSENTRY32);
	if(!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return false;
	}

	do
	{
		Core *core = haveCore(QString::fromWCharArray(pe32.szExeFile));
		if(core)
		{
			//We have a core to inject for this process, but are we already running for this pid?
			if(!hasCommunication(pe32.th32ProcessID))
			{
				//Nop we are not, so create new one and try to inject
				Sniffer *sniffer = new Sniffer(core, pe32.th32ProcessID);
				if(!isInjected(pe32.th32ProcessID, core) && !inject(core, pe32.th32ProcessID))
					delete sniffer;
				else
				{
					core->addPid(pe32.th32ProcessID);
					_communications.push_back(sniffer);
					QMetaObject::invokeMethod(gui, "registerPacketView", Q_ARG(Sniffer*, sniffer));
					emit layoutChanged();
				}			
			}
		}
	}while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return true;
}

bool Cores::inject(Core *core, int pid)
{
	HANDLE process = NULL;

	if(!(process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid)))
		return false;

	void *allocatedMemory = VirtualAllocEx(process, NULL, core->getFullPath().size() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if(allocatedMemory == NULL)
		return false;

	SIZE_T writedBytes;
	WriteProcessMemory(process, allocatedMemory, core->getFullPath().toStdString().c_str(), core->getFullPath().size() + 1, &writedBytes);

	if(writedBytes < core->getFullPath().size() + 1)
		return false;

	HANDLE thread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"), allocatedMemory, 0, NULL);
	if(thread == NULL)
		return false;

	return true;
}

//Table view
int Cores::columnCount(const QModelIndex &parent) const
{
	return COLUMNS;
}

int Cores::rowCount(const QModelIndex &parent) const
{
	return _cores.count();
}

QVariant Cores::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.row() >= _cores.count() || index.column() >= COLUMNS || (role != Qt::DisplayRole && role != Qt::DecorationRole) )
		return QVariant();

	return _cores.at(index.row())->getField(index.column());
}

QVariant Cores::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
		return QVariant();

	switch(section)
	{
	case 0:
		return QString(tr("Active"));
	case 1:
		return QString(tr("Injects"));
	case 2:
		return QString(tr("Name"));
	default:
		return QString();
	}
}