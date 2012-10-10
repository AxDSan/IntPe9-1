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
#pragma warning(disable: 4733)

#include <Skeleton.h>
#include <Crypto/Blowfish.h>
#include <General/General.h>
#include <General/Memory.h>
#include <enet/enet.h>

#include <map>
#include <vector>

using std::map;
using std::vector;

//LoL typedefs
typedef char         (__thiscall *SendPacket)(void *p, size_t length, const void *data, uint8 channel, ENetPacketFlag type);
typedef ENetEvent*   (__thiscall *AddEvent)(void *p, ENetEvent *event);
typedef void*        (__cdecl *EnetMalloc)(size_t Size);

//Some static variables and buffers
static MessagePacket *sendBuf;
static MessagePacket *recvBuf;
extern EnetMalloc enetMalloc;
extern SendPacket lolSendPacket;
extern AddEvent lolAddEvent;

#pragma pack(push,1)
struct ChatPacket
{
	enum : uint32
	{
		MYSELF = 0,
		NONAME = 100,
	};

	enum Type : uint32
	{
		ALL = 0,
		LOCAL = 1,
	};

	uint32 playerId;
	Type type;
	uint32 size;
	uint8 text;
	uint8 *getText()
	{
		return &text;
	}

	static ChatPacket *create(uint8 *text, uint32 length)
	{
		ChatPacket *packet = (ChatPacket*)malloc(sizeof(ChatPacket)+length);
		packet->playerId = MYSELF;
		packet->type = ALL;
		packet->size = length;
		memcpy(packet->getText(), text, length+1); //including the 0 byte copy
		return packet;
	}

	//Done by LoL code
	void destroy()
	{
		free((void*)this);
	}

	static uint8 getChannel()
	{
		return 5;
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
	void parsePython(const char *script);
	void debugToChat(uint8 *text);

	bool installProxy(const char *myPath){return false;};
	//Static part
	//Custom functions/callbacks
	static void onExit();
	static void addEvent(void *pointer, ENetEvent *event);
	static void recvPacket(uint8 *data, uint32 length, uint8 channel, bool ignore = false, ENetPacketFlag type = ENET_PACKET_FLAG_NO_ALLOCATE);
	static void sendPacket(uint8* data, uint32 length, uint8 channel, ENetPacketFlag type = ENET_PACKET_FLAG_RELIABLE);

	//LoL steal functions
	static void __stdcall stealRecvPacket(ENetEvent *event);
	static void __stdcall stealAddEvent(void *pointer, ENetEvent *event);
	static void __stdcall stealSendPacket(void *pointer, uint8* data, uint32 length, uint8 channel, ENetPacketFlag flag);

	//LoL variables
	static ENetPeer *addEventPeer;
	static void *pointerAddEvent;
	static void *pointerSendPacket;
	static bool isInjected;
};

extern LeagueOfLegends *leagueOfLegends;

#endif