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
#ifndef LEAGUE_OF_LEGENDS_H
#define LEAGUE_OF_LEGENDS_H

#include <Skeleton.h>
#include <Crypto/Blowfish.h>
#include <General/General.h>
#include <enet/enet.h>

#include <map>
using std::map;

//Variables
static bool doFirst = true;
static MessagePacket *sendBuf;
static MessagePacket *recvBuf;
static map<uint16, MessagePacket*> reassemble;

class LeagueOfLegends : public Skeleton
{
private:
	uint16 _keySize;
	uint8 *_key;
	bool _wrongCommandLine;
public:
	Blowfish *blowfish;
	LeagueOfLegends();

	void initialize();
	void finalize();
	void parsePython(const char *script){};
};

extern LeagueOfLegends *leagueOfLegends;

#endif