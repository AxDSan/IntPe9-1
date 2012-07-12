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