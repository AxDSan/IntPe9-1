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
#include "Stollmann.h"

/** Core information begin **/
VersionNo Stollmann::versionNo(1, 0);
char *Stollmann::name = "Stollmann proxy R04";
char *Stollmann::process = "NFCTester.exe";
bool Stollmann::hasProcess = false;
bool Stollmann::hasPython = false;
bool Stollmann::isProxy = true;
/** Core information end **/

Stollmann *stollmann = NULL;
BOOL APIENTRY DllMain(HANDLE thisHandle, DWORD callReason, LPVOID reserved)
{
	switch(callReason)
	{
	case DLL_PROCESS_ATTACH:
		stollmann = new Stollmann();
		stollmann->initialize();
		break;
	case DLL_PROCESS_DETACH:
		stollmann->finalize();
		break;
	}

	return true;
}