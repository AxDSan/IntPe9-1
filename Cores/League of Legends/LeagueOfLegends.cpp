#include "LeagueOfLegends.h"

bool doFirst = true;
MessagePacket *sendBuf;
MessagePacket *recvBuf;
map<uint16, MessagePacket*> reassemble;

//Initial setting and functions
ENetPeer *LeagueOfLegends::addEventPeer = NULL;
void *LeagueOfLegends::pointerAddEvent = NULL;
void *LeagueOfLegends::pointerSendPacket = NULL;

EnetMalloc enetMalloc;
SendPacket lolSendPacket;
AddEvent lolAddEvent;

//All signatures
//Deadbeef search place holder signature (B8 EF BE AD DE)
uint8 signatureDeadbeef[] = {0xB8, 0xEF, 0xBE, 0xAD, 0xDE};

//SendPacket (char __thiscall sendPacket(NetClient *this, size_t length, const void *data, unsigned __int8 channel, int type)) (55 8B EC 83 E4 F8 51 8B 45 14 83 E8 00)
uint8 signatureSendPacket[] = {0x55,0x8B,0xEC,0x83,0xE4,0xF8,0x51,0x8B,0x45,0x14,0x83,0xE8,0x00};

//AddEvent hook (ENetEvent *__userpurge addEvent<eax>(struct_a1 *a1<esi>, ENetEvent *a2)) (8B 46 10 83 C0 01 39 46 08 53 8B 5C 24 08)
uint8 signatureAddEvent[] = {0x8B, 0x46, 0x10, 0x83, 0xC0, 0x01, 0x39, 0x46, 0x08, 0x53, 0x8B, 0x5C, 0x24, 0x08};

//Custom enet malloc function, its thread safe! (void *__cdecl enetMalloc(size_t Size)) (68 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? A1 ?? ?? ?? ?? B9 01 00 00 00 01 0D)
uint8 maskEnetMalloc[] = "x????xx????x????xxxxxxx";
uint8 signatureEnetMalloc[] = {0x68, 0, 0, 0, 0, 0xFF, 0x15, 0, 0, 0, 0, 0xA1, 0, 0, 0, 0, 0xB9, 0x01, 0x00, 0x00, 0x00, 0x01, 0x0D};

//Meastro cleanup, always called when you kill LoL or shutdown (int __thiscall maestroCleanup(void *this)) (51 8B 0D ?? ?? ?? ?? 8B 01 8B 90 B4 00 00 00 FF)
uint8 maskMaestroCleanup[] = "xxx????xxxxxxxxx";
uint8 signatureMaestroCleanup[] = {0x51, 0x8B, 0x0D, 0, 0, 0, 0, 0x8B, 0x01, 0x8B, 0x90, 0xB4, 0x00, 0x00, 0x00, 0xFF};

//Start of LeagueOfLegends.cpp
char *LeagueOfLegends::getName()
{
	return "League of Legends.dll";
}

LeagueOfLegends::LeagueOfLegends()
{
	//Blowfish key extraction
	char *cmd = GetCommandLineA();
	uint8 q = 0, sCur = 0;
	uint16 sStart = 0, sEnd = 0;
	for(uint16 i = 0; i < strlen(cmd); i++)
	{
		if(q >= 9)
		{
			if(cmd[i] == ' ')
				sCur++;
			if(sCur == 1)
				sStart = i+2;
			if(sCur == 2)
				sEnd = i+1;
		}
		else
			if(cmd[i] == '"')
				q++;
	}
	//Test if we where able to parse the command line
	_wrongCommandLine = (!sStart || !sEnd);
	if(_wrongCommandLine)
	{
		DbgPrint("We where not able to parse the command line: %s", cmd);
		return;
	}

	_keySize = sEnd-sStart;
	_key = (uint8*)malloc(_keySize+1);
	uint8 *_keyDecrypted = (uint8*)malloc(_keySize*2);
	memcpy((char*)_key, &cmd[sStart], _keySize);

	//Create blowfish
	_key[_keySize] = 0x00;
	DbgPrint("The base64 key is: %s", _key);

	uint32 size = decode_base64(_keyDecrypted, (const char*)_key);

	int8 hexKey[25];
	for(int i = 0; i < 8; i++)
		sprintf_s(&hexKey[i*3], 25-(i*3), "%02X ", (uint8)_keyDecrypted[i]);
	hexKey[24] = 0x00;
	DbgPrint("The hex key: %s", hexKey);

	blowfish = new Blowfish(_keyDecrypted, size);

	free(_key);
	free(_keyDecrypted);
}


void LeagueOfLegends::parsePython(uint8 *script, uint32 length)
{
	DbgPrint("Creating ChatPacket: %s, (%i)", script, length);
	ChatPacket *packet = ChatPacket::create(script, length);
	packet->type = 0;

	sendPacket((uint8*)packet, packet->totalLength(), 5);
	recvPacket((uint8*)packet, packet->totalLength(), 5);

	DbgPrint("Done");
}

void LeagueOfLegends::sendPacket(uint8* data, uint32 length, uint8 channel, ENetPacketFlag type)
{
	if(pointerSendPacket == NULL)
		return;
	lolSendPacket(pointerSendPacket, length, data, channel, type);
}

void LeagueOfLegends::recvPacket(uint8 *data, uint32 length, uint8 channel, ENetPacketFlag type)
{
	if(pointerAddEvent == NULL || addEventPeer == NULL)
		return;

	leagueOfLegends->blowfish->Encrypt(data, length-(length%8));

	ENetPacket *packet = (ENetPacket*)enetMalloc(sizeof(ENetPacket));
	ENetEvent event;
	
	packet->data = data;
	packet->dataLength = length;
	packet->flags = type;
	packet->freeCallback = NULL;
	packet->referenceCount = 0;

	event.channelID = channel;
	event.type = ENET_EVENT_TYPE_RECEIVE;
	event.data = NULL;
	event.packet = packet;
	event.peer = addEventPeer;

	addEvent(pointerAddEvent, &event);
}

void LeagueOfLegends::addEvent(void *pointer, ENetEvent *event)
{
	__asm
	{
		push event
		mov esi, pointer
		call lolAddEvent
	}
}

void LeagueOfLegends::onExit()
{
	leagueOfLegends->DbgPrint("League of Legends engine stopping");
	leagueOfLegends->isRunning = false;
}

//Steal functions
void LeagueOfLegends::stealAddEvent(void *pointer, ENetEvent *event)
{
	if(pointerAddEvent == NULL && pointer != NULL)
	{
		pointerAddEvent = pointer;
		addEventPeer = event->peer;
		leagueOfLegends->DbgPrint("Stolen the addEvent arguments, ready to inject custom ENetEvents (%08X, %08X).", pointer, event->peer);
	}
}


void LeagueOfLegends::stealSendPacket(void *pointer, uint8* data, uint32 length, uint8 channel, ENetPacketFlag flag)
{
	if(pointerSendPacket == NULL)
	{
		pointerSendPacket = pointer;
		leagueOfLegends->DbgPrint("Stolen the sendPacket argument, ready to send custom packets (%08X).", pointer);
	}

	leagueOfLegends->DbgPrint("Sendhook, this: %08X, data: %08X, length: %i, channel: %i, flags: %i", pointer, data, length, channel, flag);
}

//Asm functions
static NAKED void AsmAddEvent()
{
	__asm
	{
		push ebp
		mov ebp, esp
		pushad
		PUSH DWORD PTR [ebp+0x0C]  //ENetEvent*
		PUSH esi                   //this pointer
		CALL LeagueOfLegends::stealAddEvent
		popad
		pop ebp
		mov eax,[esi+0x10]
		add eax,1
		RET
	}
}

static NAKED void ASMSendPacket()
{
	__asm
	{
		PUSHAD
		PUSH DWORD PTR [EBP + 0x14]  //a5
		PUSH DWORD PTR [EBP + 0x10]  //CHANNEL
		PUSH DWORD PTR [EBP + 0x8]   //SIZE
		PUSH DWORD PTR [EBP + 0x0C]  //DATA
		PUSH ecx                     //this pointer
		CALL LeagueOfLegends::stealSendPacket
		POPAD
		MOV EAX,DWORD PTR SS:[EBP + 0x14]
		SUB EAX,0
		RET
	}
}


static NAKED void AsmMaestroCleanup()
{
	//Hooked +18
	__asm
	{
		pushad
		call LeagueOfLegends::onExit
		popad
		mov edx,[eax+0xB4]
		ret
	}
}

void LeagueOfLegends::initialize()
{
	if(_wrongCommandLine)
		return;

	//Search section size/code
	MemorySection section = Memory::searchSection(".text");
	DbgPrint(".text section : adress %p, end %p\n", section.adress, section.adress + section.length);

	//Search for all signatures
	uint8 *deadbeef;
	uint8* addressSendPacket = Memory::searchAddress(section, signatureSendPacket, sizeof(signatureSendPacket));
	uint8 *addressAddEvent = Memory::searchAddress(section, signatureAddEvent, sizeof(signatureAddEvent));
	uint8 *addressEnetMalloc = Memory::searchAddress(section, signatureEnetMalloc, maskEnetMalloc);
	uint8 *addressMaestroCleanup = Memory::searchAddress(section, signatureMaestroCleanup, maskMaestroCleanup);

	if(addressSendPacket == NULL || addressAddEvent == NULL || addressEnetMalloc == NULL || addressMaestroCleanup == NULL)
		DbgPrint("WARNING: I did not found all signatures so carefull!!");

	//Build custom pointer functions
	lolAddEvent = (AddEvent)addressAddEvent;
	lolSendPacket = (SendPacket)addressSendPacket;
	enetMalloc = (EnetMalloc)addressEnetMalloc;

	DbgPrint("MaestroCleanup found at: %p", addressMaestroCleanup);
	Memory::writeCall(addressMaestroCleanup+9, (uint8*)AsmMaestroCleanup, 1);
	
	DbgPrint("SendPacket found at %p\n", addressSendPacket+7);
	Memory::writeCall(addressSendPacket+7, (uint8*)ASMSendPacket, 1);

	DbgPrint("Add event found at %p\n", addressAddEvent);
	Memory::writeCall(addressAddEvent, (uint8*)AsmAddEvent, 1);

	//First create buffers!!! then hook
	sendBuf = (MessagePacket*)new uint8[MP_MAX_SIZE];
	recvBuf = (MessagePacket*)new uint8[MP_MAX_SIZE];
	
	_oldWSASendTo = (defWSASendTo)_upx->hookIatFunction(NULL, "WSASendTo", (unsigned long)&newWSASendTo);
	_oldWSARecvFrom = (defWSARecvFrom)_upx->hookIatFunction(NULL, "WSARecvFrom", (unsigned long)&newWSARecvFrom);

	DbgPrint("League of Legends engine started!");	
}

void LeagueOfLegends::debugToChat(uint8 *text, uint32 length)
{
	ChatPacket *packet = ChatPacket::create(text, length);
	packet->type = 1;
	recvPacket((uint8*)packet, packet->totalLength(), 5);
}

void LeagueOfLegends::finalize()
{
	if(_wrongCommandLine)
		return;

	//Unhook, but we really can not risk it!
	//_upx->hookIatFunction(NULL, "WSASendTo", (unsigned long)&_oldWSASendTo);
	//_upx->hookIatFunction(NULL, "WSARecvFrom", (unsigned long)&_oldWSARecvFrom);

	delete[]sendBuf;
	delete[]recvBuf;
}

uint8 *reassemblePacket(ENetProtocol *command, uint32 length, uint32 *dataLength)
{
	uint32 fragmentNumber, fragmentCount, fragmentOffset, totalLength, startSequenceNumber, fragmentLength;

	fragmentLength = ENET_NET_TO_HOST_16(command->sendFragment.dataLength);
	startSequenceNumber = ENET_NET_TO_HOST_16(command->sendFragment.startSequenceNumber);
	fragmentNumber = ENET_NET_TO_HOST_32(command->sendFragment.fragmentNumber);
	fragmentCount = ENET_NET_TO_HOST_32(command->sendFragment.fragmentCount);
	fragmentOffset = ENET_NET_TO_HOST_32(command->sendFragment.fragmentOffset);
	totalLength = ENET_NET_TO_HOST_32(command->sendFragment.totalLength);

	//As i have no idea if these packets are getting correctly sequenced, but i hope so... else my code would crash so hard
	*dataLength = fragmentLength;
	if(fragmentNumber == 0)
	{
		MessagePacket *packet = (MessagePacket*)new uint8[totalLength+sizeof(MessagePacket)];
		packet->length = totalLength;
		reassemble[startSequenceNumber] = packet;	
	}

	MessagePacket *packet = reassemble[startSequenceNumber];
	memcpy(&packet->getData()[fragmentOffset], (uint8*)command+sizeof(ENetProtocolSendFragment), fragmentLength);

	//If reassembled, return data, else NULL
	if(fragmentNumber == fragmentCount-1)
	{
		sprintf_s(&packet->description[0], 50, "CMD: 8, Channel: %i", command->header.channelID);
		return (uint8*)packet;
	}
	else
		return NULL;
}

uint32 parseHeader(char *buffer, uint32 length)
{
	uint16 peerId, flags, headerSize;
	ENetProtocolHeader* header = (ENetProtocolHeader*)buffer;
	peerId = ENET_NET_TO_HOST_16 (header->peerID);
	flags = peerId & ENET_PROTOCOL_HEADER_FLAG_MASK;
	peerId &= ~ ENET_PROTOCOL_HEADER_FLAG_MASK;
	headerSize = (flags & ENET_PROTOCOL_HEADER_FLAG_SENT_TIME ? sizeof (ENetProtocolHeader) : (size_t) & ((ENetProtocolHeader *) 0) -> sentTime);

	return headerSize;
}

int32 parseEnet(char *buffer, uint32 length, uint8 **dataPointer, uint32 *dataLength, bool *isFragment, MessagePacket *packet)
{

	uint32 headerSize = 0;
	ENetProtocol *command = (ENetProtocol*)buffer;
	uint8 cmd = command->header.command & ENET_PROTOCOL_COMMAND_MASK;


	if((cmd == ENET_PROTOCOL_COMMAND_SEND_RELIABLE || cmd == ENET_PROTOCOL_COMMAND_SEND_UNRELIABLE || cmd == ENET_PROTOCOL_COMMAND_SEND_UNSEQUENCED))
	{
		uint32 maLen = 0;
		switch (cmd)
		{
			case ENET_PROTOCOL_COMMAND_SEND_RELIABLE:
				maLen = ENET_NET_TO_HOST_16(command->sendReliable.dataLength);
				break;
			case ENET_PROTOCOL_COMMAND_SEND_UNRELIABLE:
				maLen = ENET_NET_TO_HOST_16(command->sendUnreliable.dataLength);
				break;
			case ENET_PROTOCOL_COMMAND_SEND_UNSEQUENCED:
				maLen = ENET_NET_TO_HOST_16(command->sendUnsequenced.dataLength);
				break;
		}

		//leagueOfLegends->DbgPrint("[RECV] Parse, cmd: %i, channel: %i, size: %i", cmd, command->header.channelID, maLen);
		sprintf_s(&packet->description[0], 50, "CMD: %i, Channel: %i", cmd, command->header.channelID);
	}

	switch (command->header.command & ENET_PROTOCOL_COMMAND_MASK)
	{
		case ENET_PROTOCOL_COMMAND_SEND_RELIABLE:
			*dataLength = ENET_NET_TO_HOST_16(command->sendReliable.dataLength);
			headerSize = sizeof(ENetProtocolSendReliable);
		break;
		case ENET_PROTOCOL_COMMAND_SEND_UNRELIABLE:
			*dataLength = ENET_NET_TO_HOST_16(command->sendUnreliable.dataLength);
			headerSize = sizeof(ENetProtocolSendUnreliable);
		break;
		case ENET_PROTOCOL_COMMAND_SEND_UNSEQUENCED:
			*dataLength = ENET_NET_TO_HOST_16(command->sendUnsequenced.dataLength);
			headerSize = sizeof(ENetProtocolSendUnsequenced);
		break;
		case ENET_PROTOCOL_COMMAND_SEND_FRAGMENT:
			*isFragment = true;
			*dataLength = ENET_NET_TO_HOST_16(command->sendFragment.dataLength);
			headerSize = sizeof(ENetProtocolSendFragment);
			*dataPointer = reassemblePacket(command, length, dataLength);
		break;


		//All enet specific stuff, so ignore it
		case ENET_PROTOCOL_COMMAND_ACKNOWLEDGE:
		case ENET_PROTOCOL_COMMAND_CONNECT:
		case ENET_PROTOCOL_COMMAND_VERIFY_CONNECT:
		case ENET_PROTOCOL_COMMAND_DISCONNECT:
		case ENET_PROTOCOL_COMMAND_PING:
		case ENET_PROTOCOL_COMMAND_BANDWIDTH_LIMIT:
		case ENET_PROTOCOL_COMMAND_THROTTLE_CONFIGURE:
		default:
			return -1;
	}
	if(headerSize > 0 && !(*isFragment))
	{
		*dataPointer = (uint8*)buffer + headerSize;
		return headerSize + *dataLength;
	}
	if((*isFragment) && dataPointer != NULL)
	{
		return headerSize + *dataLength;
	}
	return -1;
}

/** IMPORTANT
 * Enet sends all headers and buff separated, lucky for us!
 * lpBuffer[0] = ENetProtocolHeader
 * lpBuffer[1,3,5...] = ENetProtocol
 * lpBuffer[2,4,6...] = Packet data
 */
int WSAAPI newWSASendTo(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr *lpTo, int iToLen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	if(leagueOfLegends->isAlive)
		if(dwBufferCount >= 3)
		{
			//Loop if we have 2 buffers left
			for(uint32 i = 1; i <= dwBufferCount-2; i+=2)
			{
				ENetProtocol *command = (ENetProtocol*)(lpBuffers[i].buf);
				uint8 cmd = command->header.command & ENET_PROTOCOL_COMMAND_MASK;

				//Skip if not a data packet
				if(!(cmd == ENET_PROTOCOL_COMMAND_SEND_RELIABLE || cmd == ENET_PROTOCOL_COMMAND_SEND_UNRELIABLE || cmd == ENET_PROTOCOL_COMMAND_SEND_UNSEQUENCED))
					continue;

				sprintf_s(&sendBuf->description[0], 50, "CMD: %i, Channel: %i", cmd, command->header.channelID);
		
				sendBuf->type = WSASENDTO;
				sendBuf->length = lpBuffers[i+1].len;
				memcpy(sendBuf->getData(), lpBuffers[i+1].buf, sendBuf->length);
				if(sendBuf->length >= 8)
					if(!doFirst)
						leagueOfLegends->blowfish->Decrypt(sendBuf->getData(), sendBuf->length-(sendBuf->length%8));
					else
						doFirst = false;
				leagueOfLegends->sendMessagePacket(sendBuf);
			}

		}
	return leagueOfLegends->_oldWSASendTo(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpTo, iToLen, lpOverlapped, lpCompletionRoutine);
}

int WSAAPI newWSARecvFrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	int returnLength = leagueOfLegends->_oldWSARecvFrom(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpFrom, lpFromlen, lpOverlapped, lpCompletionRoutine);

	if(leagueOfLegends->isAlive)
		if(returnLength == 0)
		{
			uint32 totalLength = *lpNumberOfBytesRecvd;
			char *buffer = lpBuffers[0].buf;		
			uint32 processed;
			
			processed = parseHeader(buffer, totalLength);
			while(processed < totalLength)
			{
				uint8 *data = NULL;
				bool isFragment = false;
				uint32 dataLength = 0;
				int32 parsed;
			
				parsed = parseEnet(&buffer[processed], totalLength, &data, &dataLength, &isFragment, recvBuf);
				if(processed == 0 || parsed < 0)
					break; //Not interesting packet

				processed += parsed;
				if(data != NULL && dataLength > 0)
				{
					if(isFragment)
					{
						MessagePacket *total = (MessagePacket*)data;
						total->type = WSARECVFROM;
						if(total->length >= 8)
							leagueOfLegends->blowfish->Decrypt(total->getData(), total->length-(total->length%8));
						leagueOfLegends->sendMessagePacket(total);
						delete []total;
					}
					else
					{
						recvBuf->type = WSARECVFROM;
						recvBuf->length = dataLength;
						memcpy(recvBuf->getData(), data, recvBuf->length);
						if(recvBuf->length >= 8)
							leagueOfLegends->blowfish->Decrypt(recvBuf->getData(), recvBuf->length-(recvBuf->length%8));
						leagueOfLegends->sendMessagePacket(recvBuf);
					}
				}

			}
		}

	return returnLength;
}