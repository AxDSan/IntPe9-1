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
#include "Injector.h"

//Needed for process injection
#include <Windows.h>
#include <Psapi.h>
#include <tlhelp32.h>

//Constants
uint32 Injector::getTimeout()
{
	return 50;
}

Injector::Injector(Manager *manager, QWidget *parent)
{
	//Build processList gui
	_parent = parent;
	_processGui = new QDialog(_parent, Qt::Dialog);
	_processView.setupUi(_processGui);
	connect(_processView.buttonRefresh, SIGNAL(clicked()), this, SLOT(refreshProcessList()), Qt::DirectConnection);
	connect(_processView.listProcess, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(selectedProcess(const QModelIndex &)), Qt::DirectConnection);

	//Set some defaults
	_manager = manager;
	
	//Move to thread
	_thread = new QThread();
	moveToThread(_thread);

	//Setup the connection to manage this thread
	connect(_thread, SIGNAL(started()), this, SLOT(start()));
	connect(this, SIGNAL(finished()), _thread, SLOT(quit()));
	connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
	connect(_thread, SIGNAL(finished()), _thread, SLOT(deleteLater()));
	
	//Start the thread and let the this->start() handle the real initial work
	_thread->start();

	DebugPrint("Injector thread running.");

	// Get debug privilege for opening protected processes
	if(enableDebugPrivilege())
		DebugPrint("Got SeDebugPrivilege");
	else
		DebugPrint("Failed to get SeDebugPrivilege");
}

Injector::~Injector()
{
	delete _processGui;
}

void Injector::start()
{
	_eventTimer = new QTimer();
	connect(_eventTimer, SIGNAL(timeout()), this, SLOT(eventLoop()));
	_eventTimer->start(getTimeout());
}

void Injector::stop()
{
	_eventTimer->blockSignals(true);  //Disable any further call to eventLoop
	emit finished();                  //Send the stop signal to thread and myself
}

void Injector::eventLoop()
{
	injectAll();     //Inject cores into new processes
}

QPixmap Injector::getIcon(uint32 pid)
{
	wchar_t filePath[MAX_PATH];
	HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
	if(process != INVALID_HANDLE_VALUE && process != 0)
	{
		DWORD size = MAX_PATH*sizeof(wchar_t);
		QueryFullProcessImageName(process, 0, filePath, &size);
		HICON icoHandle = ExtractIcon((HINSTANCE)process, filePath, 0);
		CloseHandle(process);

		if((int)icoHandle > 1)
		{
			QPixmap img = QPixmap::fromWinHICON(icoHandle);
			DestroyIcon(icoHandle);
			return img;
		}
	}
	QPixmap img(16, 1);
	img.fill(QColor(0,0,0,0));
	return img;
}

void Injector::refreshProcessList()
{
	ProcessList list = getProcesses();
	
	//Delete all items in it
	for(uint32 i = 0; i < _processView.listProcess->count(); i++)
		delete _processView.listProcess->item(i);
	_processView.listProcess->clear();

	//Make the new list
	QString name;
	ProcessList::iterator it = list.begin();
	while(it != list.end())
	{
		QListWidgetItem *item = new QListWidgetItem(QIcon(getIcon(it.key())), name.sprintf("%08X %s", it.key(), it.value().toStdString().c_str()));
		item->setData(Qt::WhatsThisRole, it.key());
		item->setData(Qt::ToolTipRole, it.value());
		_processView.listProcess->addItem(item);
		++it;
	}
}

bool Injector::enableDebugPrivilege()
{
	HANDLE hToken = NULL;
	TOKEN_PRIVILEGES tokenPriv;
	LUID luidDebug;
	if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken) != FALSE) 
	{
		if(LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luidDebug) != FALSE)
		{
			tokenPriv.PrivilegeCount           = 1;
			tokenPriv.Privileges[0].Luid       = luidDebug;
			tokenPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if(AdjustTokenPrivileges(hToken, FALSE, &tokenPriv, 0, NULL, NULL) != FALSE)
				return true;
			else
				return false;
		}
	}
	CloseHandle(hToken);
	return false;
}

void Injector::selectProcess(const QModelIndex &index)
{
	CoreList *model = (CoreList*)index.model();
	Core *core =  model->getCoreAt(index.row());
	if(!core->isProxy())
	{
		_selectedCore = core;
		refreshProcessList();
		_processGui->show();
	}
	else
		DebugPrint("Cant inject a proxy dll");
}

void Injector::selectedProcess(const QModelIndex &index)
{
	uint32 pid = index.model()->data(index, Qt::WhatsThisRole).toUInt();
	QString name = index.model()->data(index, Qt::ToolTipRole).toString();

	if(!isInjected(pid, _selectedCore))
		if(!inject(pid, _selectedCore))
			return DebugPrint("Failed to inject the core");

	if(!_manager->hasSniffer(pid))
	{
		Sniffer *sniffer = new Sniffer(pid, name, _selectedCore);  //Create new sniffer thread
		connect(sniffer, SIGNAL(doneLoading(Sniffer*)), _manager, SLOT(registerSniffer(Sniffer*)));
	}

	_processGui->hide();
}

/**
 * @brief	Query if the core is injected into the specific process id.
 * @author	Intline9
 * @date	18-4-2012
 * @param	pid	    	The process id
 * @param	core		The core to check for
 * @return	true if injected, false if not.
 */
bool Injector::isInjected(uint32 pid, Core *core)
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
	do{
		if(core->getModuleName() == QString::fromWCharArray(me32.szModule))
			return true;
	} while(Module32Next(hModuleSnap, &me32));

	CloseHandle(hModuleSnap);
	return false;
}

/**
 * @brief	Inject a specific core into a specific process by using create remote thread
 * 		with LoadLibraryA call.
 * @author	Intline9
 * @date	18-4-2012
 * @param	pid		The process id
 * @param	core		The core to inject
 * @return	true if the thread is started and injected, false if it fails
 */
bool Injector::inject(uint32 pid, Core *core)
{
	QString dbg = QString("Injecting core: ")+core->getName();
	DebugPrint(dbg.toStdString().c_str());

	if(core == NULL)
		return false;

	HANDLE process = NULL;

	if(!(process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid)))
	{
		DebugPrint("Failed to open proc");
		return false;
	}

	void *allocatedMemory = VirtualAllocEx(process, NULL, core->getFullPath().size() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if(allocatedMemory == NULL)
	{
		DebugPrint("Failed to alloc");
		return false;
	}

	SIZE_T writedBytes;
	WriteProcessMemory(process, allocatedMemory, core->getFullPath().toStdString().c_str(), core->getFullPath().size() + 1, &writedBytes);

	if(writedBytes < core->getFullPath().size() + 1)
	{
		DebugPrint("Failed to write bytes");
		return false;
	}

	HANDLE thread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"), allocatedMemory, 0, NULL);
	if(thread == NULL)
	{
		DebugPrint("Failed to start remote thread");
		return false;
	}

	//Cleanup
	CloseHandle(thread);
	CloseHandle(process);

	DebugPrint("Successfully injected");

	return true;
}

/**
 * @brief	Gets all running processes on the computer
 *
 * @return	ProcessList
 */
ProcessList Injector::getProcesses()
{
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;
	ProcessList processList;

	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if(hProcessSnap != INVALID_HANDLE_VALUE)
	{
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if(Process32First(hProcessSnap, &pe32))
		{
			do{
				if(pe32.th32ProcessID == GetCurrentProcessId())
					continue;
				QString name(QString::fromWCharArray(pe32.szExeFile));
				if(!name.startsWith('['))
					processList.insert(pe32.th32ProcessID, name);
			}while (Process32Next(hProcessSnap, &pe32));
			CloseHandle(hProcessSnap);
		}
	}
	return processList;
}

/**
 * @brief	Injects the cores into the required processes.
 * @note	I did not use getProcesses(), as that will make the complexity of injectAll twice as big
 * @author	Intline9
 * @date	18-4-2012
 * @return	true if it succeeds, false if it fails.
 */
bool Injector::injectAll()
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

	//Walk through all processes
	do
	{
		if(_injected.find(pe32.th32ProcessID) == _injected.end())                       //If we already injected to this pid skip it!
		{
			QString processName = QString::fromWCharArray(pe32.szExeFile);
			Core *core = _manager->getCore(processName);                                //If we have a Core for this process
			if(core)
			{
				// If the core is not a proxy inject else just start sniffing
				if(!core->isProxy())
				{
					QString dbg = QString("Found an injectable exe: ") + processName;
					DebugPrint(dbg.toStdString().c_str());
					if(!isInjected(pe32.th32ProcessID, core))                           //Check if we already injected
						if(!inject(pe32.th32ProcessID, core))                           //Not yet injected so try now
							continue;                                                   //We failed to inject so skip this injection (and try again later)
				}

				_injected.insert(pe32.th32ProcessID);                                   //Remember that we injected to this process
				if(!_manager->hasSniffer(pe32.th32ProcessID))                           //Check if there is already an active sniffer for this pid
				{
					Sniffer *sniffer = new Sniffer(pe32.th32ProcessID, processName, core);                      //Create new sniffer thread
					connect(sniffer, SIGNAL(doneLoading(Sniffer*)), _manager, SLOT(registerSniffer(Sniffer*))); //Set up default connections
				}
			}
		}
	}while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return true;
}