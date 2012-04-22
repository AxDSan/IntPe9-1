#include "LeagueOfLegends.h"

bool doFirst = true;
MessagePacket *sendBuf;
MessagePacket *recvBuf;
map<uint16, MessagePacket*> reassemble;

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
		DbgPrint("We where not able to parse the commandline: %s", cmd);
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

void LeagueOfLegends::initialize()
{
	if(_wrongCommandLine)
		return;

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
	PacketQue *p = new PacketQue((uint8*)packet, packet->totalLength());
	packets.push_back(p);
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
				leagueOfLegends->sendPacket(sendBuf);
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
						leagueOfLegends->sendPacket(total);
						delete []total;
					}
					else
					{
						recvBuf->type = WSARECVFROM;
						recvBuf->length = dataLength;
						memcpy(recvBuf->getData(), data, recvBuf->length);
						if(recvBuf->length >= 8)
							leagueOfLegends->blowfish->Decrypt(recvBuf->getData(), recvBuf->length-(recvBuf->length%8));
						leagueOfLegends->sendPacket(recvBuf);
					}
				}

			}

			//Currently procssed points to offset where we can inject an extra packet
			while(leagueOfLegends->packets.size() > 0)
			{
				leagueOfLegends->DbgPrint("Trying to inject a packet");
				PacketQue *packet = leagueOfLegends->packets.at(leagueOfLegends->packets.size()-1);

				ENetProtocolSendReliable payload;
				payload.header.channelID = 3;
				payload.header.command = ENET_PROTOCOL_COMMAND_SEND_RELIABLE;
				payload.header.reliableSequenceNumber = 0;
				payload.dataLength = packet->length;
				memcpy(&buffer[processed], &payload, sizeof(ENetProtocolSendReliable));
				*lpNumberOfBytesRecvd += sizeof(ENetProtocolSendReliable);
				processed+= sizeof(ENetProtocolSendReliable);
				memcpy(&buffer[processed], packet->data, packet->length);
				*lpNumberOfBytesRecvd += packet->length;
				processed += packet->length;
				delete packet;
				leagueOfLegends->packets.pop_back();
				leagueOfLegends->DbgPrint("Injected a packet");
			}
		}

	return returnLength;
}