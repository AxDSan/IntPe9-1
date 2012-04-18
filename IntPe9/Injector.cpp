#include "Injector.h"

//Needed for process injection
#include <Windows.h>
#include <tlhelp32.h>

//Constants
uint32 Injector::getTimeout()
{
	return 50;
}


Injector::Injector(Manager *manager)
{
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

	do
	{
		if(core->getBaseName() == QString::fromWCharArray(me32.szModule))
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
	if(core == NULL)
		return false;

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

	//Cleanup
	//VirtualFreeEx(process, allocatedMemory, 0,  MEM_RELEASE);
	CloseHandle(thread);
	CloseHandle(process);

	return true;
}

/**
 * @brief	Injects the cores into the required processes.
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
		if(_injected.find(pe32.th32ProcessID) == _injected.end())                         //If we already injected to this pid skip it!
		{
			Core *core = _manager->getCore(QString::fromWCharArray(pe32.szExeFile));  //If we have a Core for this process
			if(core)
			{
				if(!isInjected(pe32.th32ProcessID, core))                         //Check if we already injected
				{
					if(!inject(pe32.th32ProcessID, core))                     //Not yet injected so try now
						continue;                                         //We failed to inject so skip this injection (and try again later)
				}
				else
				{
					_injected.insert(pe32.th32ProcessID);                     //Remember that we injected to this process
					Sniffer *sniffer = new Sniffer(pe32.th32ProcessID, core); //Create new sniffer thread

					//Set up default connections
					connect(sniffer, SIGNAL(doneLoading(Sniffer*)), _manager, SLOT(registerSniffer(Sniffer*)));
				}
			}
		}
	}while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return true;
}