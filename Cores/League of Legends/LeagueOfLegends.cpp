#include "LeagueOfLegends.h"
#include "PythonWrapper.h"

MessagePacket *sendBuf;
MessagePacket *recvBuf;

//Initial setting and functions
bool LeagueOfLegends::isInjected = false;
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

//RecvPacket ESP+1C = ENetEvent* (8B 7C 24 2C 85 FF ?? ?? ?? ?? ?? ?? 8B)
uint8 maskRecvPacket[] = "xxxxxx??????x";
uint8 signatureRecvPacket[] = {0x8B, 0x7C, 0x24, 0x2C, 0x85, 0xFF, 0, 0, 0, 0, 0, 0, 0x8B};

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


void LeagueOfLegends::parsePython(const char *script)
{
	try
	{
		exec(script, pythonNamespace);
	}
	catch(boost::python::error_already_set const &)
	{
		PyErr_Print();
		boost::python::object sys(boost::python::handle<>(PyImport_ImportModule("sys")));
		boost::python::object err = sys.attr("stderr");
		std::string errorText = boost::python::extract<std::string>(err.attr("getvalue")());
		debugToChat((uint8*)errorText.c_str());
		PyRun_SimpleString("sys.stderr = cStringIO.StringIO()");
	}
}

void LeagueOfLegends::sendPacket(uint8* data, uint32 length, uint8 channel, ENetPacketFlag type)
{
	if(pointerSendPacket == NULL)
		return;

	isInjected = true;
	lolSendPacket(pointerSendPacket, length, data, channel, type);
}

void LeagueOfLegends::recvPacket(uint8 *data, uint32 length, uint8 channel, bool ignore, ENetPacketFlag type)
{
	if(pointerAddEvent == NULL || addEventPeer == NULL)
		return;

	if(length >= 8)
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
	event.data = (ignore) ? INJECT_IGNORE : INJECT_RECV;
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
	//leagueOfLegends->DbgPrint("Send event, type: %i, channel: %i, peer: %08X, data: %08X, packet: %08X", event->type, event->channelID, event->peer, event->data, event->packet);
}

void LeagueOfLegends::stealRecvPacket(ENetEvent *event)
{	
	//Skip all non receive events
	if(event->type != ENET_EVENT_TYPE_RECEIVE)
		return;

	//Skip ignore packets
	if(event->data == INJECT_IGNORE)
		return;

	//event->packet = NULL; to block a packet

	//Copy the data and send it to front end
	ENetPacket *packet = event->packet;
	if(event->data == INJECT_RECV)
	{
		recvBuf->type = INJECT_RECV;
		event->data = NULL;
	}
	else
		recvBuf->type = WSARECVFROM;

	recvBuf->length = packet->dataLength;
	memcpy(recvBuf->getData(), packet->data, recvBuf->length);
	sprintf_s(&recvBuf->description[0], 50, "Channel: %i, Flag: %i", event->channelID, packet->flags);
	leagueOfLegends->sendMessagePacket(recvBuf);
}

void LeagueOfLegends::stealSendPacket(void *pointer, uint8* data, uint32 length, uint8 channel, ENetPacketFlag flag)
{
	if(pointerSendPacket == NULL)
	{
		pointerSendPacket = pointer;
		leagueOfLegends->DbgPrint("Stolen the sendPacket argument, ready to send custom packets (%08X).", pointer);
	}

	//Copy the data and send it to front end
	if(isInjected)
	{
		sendBuf->type = INJECT_SEND;
		isInjected = false;
	}
	else
		sendBuf->type = WSASENDTO;

	sendBuf->length = length;
	memcpy(sendBuf->getData(), data, sendBuf->length);
	sprintf_s(&sendBuf->description[0], 50, "Channel: %i, Flag: %i", channel, flag);
	leagueOfLegends->sendMessagePacket(sendBuf);
}

//Asm functions
static NAKED void AsmRecvPacket()
{
	__asm
	{
		mov eax, esp
		add eax, 0x20
		push eax //ENetEvent*
		call LeagueOfLegends::stealRecvPacket
		mov edi,[esp+0x30]
		test edi,edi
		RET
	}
}

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
	__asm
	{
		pushad
		call LeagueOfLegends::onExit
		popad
		mov edx,[eax+0xB4]
		RET
	}
}


void LeagueOfLegends::debugToChat(uint8 *text)
{
	ChatPacket *packet = ChatPacket::create(text, strlen((const char*)text));
	DbgPrint("Debug chat: %s", text);
	packet->playerId = ChatPacket::NONAME;
	packet->type = ChatPacket::LOCAL;
	recvPacket((uint8*)packet, packet->totalLength(), ChatPacket::getChannel(), true);
}

BOOST_PYTHON_MODULE(lol)
{
	boost::python::class_<PythonWrapper>("Pe")
		.def("recvPacket", &PythonWrapper::recvPacket)
		.def("sendPacket", &PythonWrapper::sendPacket)
		.def("sendChat", &PythonWrapper::sendChat)
		.def("write", &PythonWrapper::write);

	def("getInstance", &PythonWrapper::getInstance, boost::python::return_value_policy<boost::python::reference_existing_object>());
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
	uint8* addressRecvPacket = Memory::searchAddress(section, signatureRecvPacket, maskRecvPacket);
	uint8 *addressAddEvent = Memory::searchAddress(section, signatureAddEvent, sizeof(signatureAddEvent));
	uint8 *addressEnetMalloc = Memory::searchAddress(section, signatureEnetMalloc, maskEnetMalloc);
	uint8 *addressMaestroCleanup = Memory::searchAddress(section, signatureMaestroCleanup, maskMaestroCleanup);

	if(addressSendPacket == NULL || addressAddEvent == NULL || addressEnetMalloc == NULL || addressMaestroCleanup == NULL || addressRecvPacket == NULL)
		DbgPrint("WARNING: I did not found all signatures so carefully!!");

	//First create buffers!!! then hook
	sendBuf = (MessagePacket*)new uint8[MP_MAX_SIZE];
	recvBuf = (MessagePacket*)new uint8[MP_MAX_SIZE];

	//Set function
	lolAddEvent = (AddEvent)addressAddEvent;
	lolSendPacket = (SendPacket)addressSendPacket;
	enetMalloc = (EnetMalloc)addressEnetMalloc;

	//Set hooks
	Memory::writeCall(addressRecvPacket, (uint8*)AsmRecvPacket, 1);
	Memory::writeCall(addressMaestroCleanup+9, (uint8*)AsmMaestroCleanup, 1);
	Memory::writeCall(addressSendPacket+7, (uint8*)ASMSendPacket, 1);
	Memory::writeCall(addressAddEvent, (uint8*)AsmAddEvent, 1);
	DbgPrint("League of Legends hooks applied!");

	//Start python extending
	initlol();
	boost::python::object mainModule = boost::python::import("__main__");
	pythonNamespace = mainModule.attr("__dict__");

	PyRun_SimpleString("import cStringIO");
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("import lol");
	PyRun_SimpleString("pe = lol.getInstance()");
	PyRun_SimpleString("sys.stderr = cStringIO.StringIO()");
	PyRun_SimpleString("sys.stdout = pe");
	DbgPrint("League of Legends engine started!");
}

void LeagueOfLegends::finalize()
{
	if(_wrongCommandLine)
		return;

	delete[]sendBuf;
	delete[]recvBuf;
}

