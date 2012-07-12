#ifndef _COMMON_H_
#define _COMMON_H_

#include <iostream>
#include <vector>
#include <cstdarg>

#ifdef _DEBUG
#define DebugPrint debugPrint
#else
#define DebugPrint(...)
#endif

void debugPrint(char *str);
void debugPrint(const char *str);

#define MP_QUEUE_NAME "PacketListener_"
#define MP_QUEUE_NAME_SIZE 30
#define MP_MAX_SIZE 0x1500
#define MP_MAX_NO 0x100

#define CC_QUEUE_NAME "CommandControll_"
#define CC_QUEUE_NAME_SIZE 35
#define CC_MAX_SIZE 0x2000
#define CC_MAX_NO 0x100

typedef long long		int64;
typedef int			int32;
typedef short			int16;
typedef char			int8; 
typedef unsigned long long	uint64;
typedef unsigned int		uint32;
typedef unsigned short		uint16;
typedef unsigned char		uint8;
struct CoreInfo;

//Export functions
typedef void (*GetCoreInfo)(CoreInfo *info);

enum PacketType : uint32
{
	SEND,
	RECV,
	SENDTO,
	RECVFROM,
	WSASEND,
	WSARECV,
	WSASENDTO,
	WSARECVFROM,
	INJECT_SEND = 0xBEEF,
	INJECT_RECV = 0xDEAD,
	INJECT_IGNORE = 0xFAFA,
};

enum CommandType : uint8
{
	START,
	EXIT,
	PYTHON,
};

#pragma pack(push)
#pragma pack(1)
#pragma warning(push)
#pragma warning(disable: 4200)

struct VersionNo
{
public:

	int8 major;
	int8 minor;

	VersionNo()
	{
		major = minor = 0;
	}

	VersionNo(int8 major, int8 minor)
	{
		this->major = major;
		this->minor = minor;
	}
};

struct CoreInfo
{
public:
	int8 name[25];
	VersionNo versionNo;
	int8 process[60];
	bool hasProcess;
	bool hasPython;

	CoreInfo()
	{

	}
};

class CommandControll
{
public:
	CommandType cmd;
	uint32 length;
	uint8 data;

	CommandControll(CommandType cmd)
	{
		this->cmd = cmd;
		length = 0;
	}

	uint8 *getData()
	{
		return &data;
	}

	uint32 totalSize()
	{
		return length+sizeof(CommandControll);
	}
};

class MessagePacket
{
public:
	PacketType type;
	int8 description[50];
	uint32 length;
	uint8 data;

	uint8 *getData()
	{
		return &data;
	}

	uint32 messagePacketSize()
	{
		return length+sizeof(MessagePacket);
	}
};

#pragma warning(pop)
#pragma pack(pop)

#endif