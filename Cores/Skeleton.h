#ifndef SKELETON_H
#define SKELETON_H

//Boost
#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>

//Default system
#include <WinSock2.h>
#include <Common.h>
#include <map>
#include <string>
#include <list>

//Custom library's
#include <UpxLoader/Upx.h>

using std::map;
using std::list;
using std::string;

using namespace boost::python;
using namespace boost::interprocess;

typedef int	(WSAAPI *defWSASendTo)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr *lpTo, int iToLen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
typedef int	(WSAAPI *defWSARecvFrom)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
typedef bool	(WSAAPI *defWSAGetOverlappedResult)(SOCKET s, LPWSAOVERLAPPED lpOverlapped, LPDWORD lpcbTransfer, BOOL fWait, LPDWORD lpdwFlags);
typedef int	(WSAAPI *defWSAConnect)(SOCKET s, const struct sockaddr *name, int namelen, LPWSABUF lpCallerData, LPWSABUF lpCalleeData, LPQOS lpSQOS, LPQOS lpGQOS);
typedef int	(WSAAPI *defWSARecv)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
typedef SOCKET	(WSAAPI *defWSASocketA)(int af, int type, int protocol, LPWSAPROTOCOL_INFOA lpProtocolInfo, GROUP g, DWORD dwFlags);


int		WSAAPI newWSASendTo(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr *lpTo, int iToLen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
int		WSAAPI newWSARecvFrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
bool		WSAAPI newWSAGetOverlappedResult(SOCKET s, LPWSAOVERLAPPED lpOverlapped, LPDWORD lpcbTransfer, BOOL fWait, LPDWORD lpdwFlags);
int		WSAAPI newWSAConnect(SOCKET s, const struct sockaddr *name, int namelen, LPWSABUF lpCallerData, LPWSABUF lpCalleeData, LPQOS lpSQOS, LPQOS lpGQOS);
int		WSAAPI newWSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
SOCKET		WSAAPI newWSASocketA(int af, int type, int protocol, LPWSAPROTOCOL_INFOA lpProtocolInfo, GROUP g, DWORD dwFlags);


class Skeleton
{
protected:
	Upx *_upx;
	virtual void initialize() = 0;
	virtual void finalize() = 0;

private:
	message_queue *_masterQue, *_packetQue;
	char *_dbgPrint;
	char *_dbg;

public:
	void DbgPrint(const char* format, ...);
	Skeleton();
	~Skeleton();

	bool isActive;

	//Public functions
	bool sendCommand();
	bool sendPacket(MessagePacket *packet);

	//All hookable functions
	defWSASendTo			_oldWSASendTo;
	defWSARecvFrom			_oldWSARecvFrom;
	defWSAGetOverlappedResult	_oldWSAGetOverlappedResult;
	defWSAConnect			_oldWSAConnect;
	defWSARecv			_oldWSARecv;
	defWSASocketA			_oldWSASocketA;
};

#endif