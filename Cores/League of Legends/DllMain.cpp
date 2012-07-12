#include "LeagueOfLegends.h"

LeagueOfLegends *leagueOfLegends = NULL;
BOOL APIENTRY DllMain(HANDLE thisHandle, DWORD callReason, LPVOID reserved)
{
	try
	{
		switch(callReason)
		{
		case DLL_PROCESS_ATTACH:
			leagueOfLegends = new LeagueOfLegends();
			if(!leagueOfLegends->isFail)
				leagueOfLegends->initialize();
			break;
		case DLL_PROCESS_DETACH:
			leagueOfLegends->DbgPrint("Unloading");
			if(leagueOfLegends != NULL)
			{
				if(!leagueOfLegends->isFail)
					leagueOfLegends->finalize();
				leagueOfLegends->exit();
			}
			break;
		}
	}
	catch(...)
	{

	}

	
	return true;
}