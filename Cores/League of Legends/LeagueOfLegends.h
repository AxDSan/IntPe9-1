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
#pragma pack(push,1)
struct ChatPacket
{
	uint32 cmd;
	uint32 type;
	uint32 size;
	uint8 text;
	uint8 *getText()
	{
		return &text;
	}

	static ChatPacket *create(uint8 *text, uint32 length)
	{
		ChatPacket *packet = (ChatPacket*)malloc(sizeof(ChatPacket)+length);
		packet->cmd = 0;
		packet->type = 0;
		packet->size = length;
		memcpy(packet->getText(), text, length+1); //including the 0 byte copy
		return packet;
	}

	/* This will be done by LoL code */
	void destroy()
	{
		free((void*)this);
	}

	uint32 totalLength()
	{
		return sizeof(ChatPacket)+size;
	}
};
#pragma pack(pop)
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
	void parsePython(uint8 *script, uint32 length);
	void debugToChat(uint8 *text, uint32 length);

	vector<PacketQue*> packets;
};

extern LeagueOfLegends *leagueOfLegends;

#endif