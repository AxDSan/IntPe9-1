#ifndef SKELETON_H
#define SKELETON_H

//Boost
#include <boost/python.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>

//Default system
#include <WinSock2.h>
#include <Common.h>
#include <map>
#include <string>
#include <list>

//Custom library's
#include <UpxLoader/Upx.h>

#define DBG_SIZE 2048
#define NAKED __declspec(naked)
#define DLL_EXPORT __declspec(dllexport)

using std::map;
using std::list;
using std::string;

using namespace boost::interprocess;

typedef int	(WSAAPI *defWSASendTo)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr *lpTo, int iToLen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
typedef int	(WSAAPI *defWSARecvFrom)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
typedef bool	(WSAAPI *defWSAGetOverlappedResult)(SOCKET s, LPWSAOVERLAPPED lpOverlapped, LPDWORD lpcbTransfer, BOOL fWait, LPDWORD lpdwFlags);
typedef int	(WSAAPI *defWSAConnect)(SOCKET s, const struct sockaddr *name, int namelen, LPWSABUF lpCallerData, LPWSABUF lpCalleeData, LPQOS lpSQOS, LPQOS lpGQOS);
typedef int	(WSAAPI *defWSASend)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
typedef int	(WSAAPI *defWSARecv)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
typedef SOCKET	(WSAAPI *defWSASocketA)(int af, int type, int protocol, LPWSAPROTOCOL_INFOA lpProtocolInfo, GROUP g, DWORD dwFlags);
typedef int	(WSAAPI *defSend)(SOCKET s, const char FAR *buf, int len, int flags);

int		WSAAPI newWSASendTo(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr *lpTo, int iToLen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
int		WSAAPI newWSARecvFrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
bool		WSAAPI newWSAGetOverlappedResult(SOCKET s, LPWSAOVERLAPPED lpOverlapped, LPDWORD lpcbTransfer, BOOL fWait, LPDWORD lpdwFlags);
int		WSAAPI newWSAConnect(SOCKET s, const struct sockaddr *name, int namelen, LPWSABUF lpCallerData, LPWSABUF lpCalleeData, LPQOS lpSQOS, LPQOS lpGQOS);
int		WSAAPI newWSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
int		WSAAPI newWSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
SOCKET		WSAAPI newWSASocketA(int af, int type, int protocol, LPWSAPROTOCOL_INFOA lpProtocolInfo, GROUP g, DWORD dwFlags);
int		WSAAPI newSend(SOCKET s, const char FAR *buf, int len, int flags);

class Skeleton
{
protected:
	Upx *_upx;
	virtual void initialize() = 0;
	virtual void finalize() = 0;
	virtual void parsePython(const char *script) = 0;
	
	void startThread();
	boost::python::object pythonNamespace;

	//Module information
	static VersionNo versionNo;
	static char *name;
	static char *process;
	static bool hasProcess;
	static bool hasPython;

private:
	message_queue *_masterQue, *_packetQue;
	char *_dbgPrint;
	char *_dbg;

public:
	DLL_EXPORT void getCoreInfo(CoreInfo *info);
	void DbgPrint(const char* format, ...);
	Skeleton();
	~Skeleton();

	bool isFail;
	bool isAlive;
	bool isRunning;

	//Public functions
	bool sendCommand();
	bool sendMessagePacket(MessagePacket *packet);
	void handleCommand(CommandControll *command);
	bool stop();
	bool start();
	void exit();

	//All hookable functions with IAT
	defWSASendTo			_oldWSASendTo;
	defWSARecvFrom			_oldWSARecvFrom;
	defWSAGetOverlappedResult	_oldWSAGetOverlappedResult;
	defWSAConnect			_oldWSAConnect;
	defWSASend			_oldWSASend;
	defWSARecv			_oldWSARecv;
	defWSASocketA			_oldWSASocketA;
	defSend				_oldSend;
};

#endif