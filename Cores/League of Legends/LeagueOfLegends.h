#ifndef LEAGUE_OF_LEGENDS_H
#define LEAGUE_OF_LEGENDS_H

#include <Skeleton.h>
#include <Crypto/Blowfish.h>
#include <General/General.h>
#include <enet/enet.h>

#include <map>
using std::map;

class LeagueOfLegends : public Skeleton
{
private:
	uint16 _keySize;
	uint8 *_key;
public:
	Blowfish *blowfish;
	LeagueOfLegends();

	void initialize();
	void finalize();
	char *getName();
};

extern LeagueOfLegends *leagueOfLegends;

#endif