#ifndef LEAGUE_OF_LEGENDS_H
#define LEAGUE_OF_LEGENDS_H

#include <Skeleton.h>
#include <Crypto/Blowfish.h>
#include <General/General.h>
#include <enet/enet.h>

#include <map>
#include <vector>
using std::map;
using std::vector;

struct PacketQue
{
	PacketQue(uint8* d, uint32 l)
	{
		data = d;
		length = l;
	}

	uint8 *data;
	uint32 length;
};

struct ChatPacket
{
	uint8 cmd;
	uint8 type;
	uint8 size;
	uint8 text;
	uint8 *getText()
	{
		return &text;
	}

	static ChatPacket *create(uint8 *text, uint32 length)
	{
		ChatPacket *packet = (ChatPacket*)new uint8[sizeof(ChatPacket)+length];
		packet->cmd = 0;
		packet->type = 0;
		packet->size = length;
		memcpy(packet->getText(), text, length);
		return packet;
	}

	void destroy()
	{
		delete [](uint8*)this;
	}

	uint32 totalLength()
	{
		return sizeof(ChatPacket)+size;
	}
};

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
	char *getName();

	void debugToChat(uint8 *text, uint32 length);

	vector<PacketQue*> packets;
};

extern LeagueOfLegends *leagueOfLegends;

#endif