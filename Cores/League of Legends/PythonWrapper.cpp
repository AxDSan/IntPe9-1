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
#include "PythonWrapper.h"
#include "LeagueOfLegends.h"

PythonWrapper *PythonWrapper::instance = NULL;

PythonWrapper::PythonWrapper()
{

}

PythonWrapper::~PythonWrapper()
{

}

PythonWrapper *PythonWrapper::getInstance()
{
	if(PythonWrapper::instance == NULL)
		return PythonWrapper::instance = new PythonWrapper();
	else
		return PythonWrapper::instance;
}

uint8 *PythonWrapper::listToChars(boost::python::list &pList, uint32 *size, bool useMalloc)
{
	boost::python::stl_input_iterator<uint8> begin(pList), end;
	std::list<uint8>bytes;
	bytes.assign(begin, end);

	leagueOfLegends->DbgPrint("Got a list to chars with size: %i", bytes.size());
	*size = bytes.size();
	uint8 *buffer;
	if(useMalloc)
	{
		leagueOfLegends->DbgPrint("Allocing with: %08X enetMalloc, with arg: %i", enetMalloc, bytes.size());
		buffer = (uint8*)enetMalloc(bytes.size());
	}
	else
	{
		buffer = new uint8[bytes.size()];
	}

	leagueOfLegends->DbgPrint("Got an alloc on: %08X", buffer);

	uint32 i = 0;
	while(!bytes.empty())
	{
		buffer[i] = bytes.front();
		bytes.pop_front(); i++;
	}
	return buffer;
}

void PythonWrapper::sendPacket(boost::python::list &bytes, uint8 channel, uint32 flags)
{
	uint32 length;
	uint8 *packet = listToChars(bytes, &length);
	leagueOfLegends->sendPacket(packet, length, channel, (ENetPacketFlag)flags);
	delete []packet;
}

void PythonWrapper::recvPacket(boost::python::list &bytes, uint8 channel, uint32 flags)
{
	uint32 length;
	uint8 *packet = listToChars(bytes, &length, true);
	leagueOfLegends->recvPacket(packet, length, channel, false, (ENetPacketFlag)flags);
}

void PythonWrapper::sendChat(uint32 type, std::string const&msg)
{
	ChatPacket *packet = ChatPacket::create((uint8*)msg.c_str(), msg.length());
	packet->type = (ChatPacket::Type)type;
	leagueOfLegends->sendPacket((uint8*)packet, packet->totalLength(), ChatPacket::getChannel());
	packet->destroy();
}

void PythonWrapper::write(std::string const&msg)
{
	if(msg.size() <= 0 || msg[0] == 0x0a)
		return;
	leagueOfLegends->debugToChat((uint8*)msg.c_str());
}