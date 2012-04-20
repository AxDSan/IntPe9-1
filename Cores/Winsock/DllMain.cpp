#include "Winsock.h"

Winsock *winsock = NULL;
BOOL APIENTRY DllMain(HANDLE thisHandle, DWORD callReason, LPVOID reserved)
{
	switch(callReason)
	{
		case DLL_PROCESS_ATTACH:
			winsock = new Winsock();
			winsock->initialize();
		break;
		case DLL_PROCESS_DETACH:
			if(winsock != NULL)
			{
				winsock->finalize();
				delete winsock;
			}
		break;
	}
	return true;
}