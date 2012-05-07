#ifndef NO_PYTHON

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
		buffer = (uint8*)enetMalloc(bytes.size());
	else
		buffer = new uint8[bytes.size()];

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

#endif