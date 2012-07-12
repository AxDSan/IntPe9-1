#ifndef LOL_SIGNATURE_H
#define LOL_SIGNATURE_H

/**
League of Legends core module for LoL 0.0.0.165
----------------------------------------------------------
 Some notes on how to simple find updates in functions

 To find enet loop:
 - Search for the encryption functions: 83 EC 08 53 8B 5C 24 14  55 8B 6C 24 1C 33 C0 83 FD 08 56 57
 - First occurrence is decrypt buffer, second is encrypt buffer
 - Then for jump to xref from the decrypt buffer and the first result should be enetLoop (should be obvious by a 3 way switch and some strings about client connected etc)

 To find sendPacket:
 - Search for the encryption functions: 83 EC 08 53 8B 5C 24 14  55 8B 6C 24 1C 33 C0 83 FD 08 56 57
 - First occurrence is decrypt buffer, second is encrypt buffer
 - Then from the encrypt buffer the second X ref should be sendPacket (Begin has some if {if {}else{ if return} }else )

 */
//All signatures
//Deadbeef search place holder signature (B8 EF BE AD DE)
static unsigned char signatureDeadbeef[] = {0xB8, 0xEF, 0xBE, 0xAD, 0xDE};

//SendPacket (char __thiscall sendPacket(NetClient *this, size_t length, const void *data, unsigned __int8 channel, int type)) (55 8B EC 83 E4 F8 64 A1 00 00 00 00 6A FF 68 ?? ?? ?? ?? 50 8B 45 14)
static unsigned char maskSendPacket[] = "xxxxxxxxxxxxxxx????xxxx";
static unsigned char signatureSendPacket[] = {0x55, 0x8B, 0xEC, 0x83, 0xE4, 0xF8, 0x64, 0xA1, 0x00, 0x00, 0x00, 0x00, 0x6A, 0xFF, 0x68, 0, 0, 0, 0, 0x50, 0x8B, 0x45, 0x14};

//RecvPacket ESP+1C = ENetEvent* (39 96 E8 00 00 00 74 ?? 66 0F B6 08)
static unsigned char maskRecvPacket[] = "xxxxxxx?xxxx";
static unsigned char signatureRecvPacket[] = {0x39, 0x96, 0xE8, 0x00, 0x00, 0x00, 0x74, 0, 0x66, 0x0F, 0xB6, 0x08};

//AddEvent hook (ENetEvent *__userpurge addEvent<eax>(struct_a1 *a1<esi>, ENetEvent *a2)) (8B 46 10 83 C0 01 39 46 08 77 ?? E8)
static unsigned char maskAddEvent[] = "xxxxxxxxxx?x";
static unsigned char signatureAddEvent[] = {0x8B, 0x46, 0x10, 0x83, 0xC0, 0x01, 0x39, 0x46, 0x08, 0x77, 0, 0xE8};

//Custom enet malloc function, its thread safe! (void *__cdecl enetMalloc(size_t Size)) (51 56 68 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? A1 ?? ?? ?? ?? B9 01 00 00 00 01 0D)
static unsigned char maskEnetMalloc[] = "xxx????xx????x????xxxxxxx";
static unsigned char signatureEnetMalloc[] = {0x51, 0x56, 0x68, 0, 0, 0, 0, 0xFF, 0x15, 0, 0, 0, 0, 0xA1, 0, 0, 0, 0, 0xB9, 0x01, 0x00, 0x00, 0x00, 0x01, 0x0D};

//Meastro cleanup, always called when you kill LoL or shutdown (int __thiscall maestroCleanup(void *this)) (51 8B 0D ?? ?? ?? ?? 8B 01 8B 90 ?? 00 00 00 FF)
static unsigned char maskMaestroCleanup[] = "xxx????xxxx?xxxx";
static unsigned char signatureMaestroCleanup[] = {0x51, 0x8B, 0x0D, 0, 0, 0, 0, 0x8B, 0x01, 0x8B, 0x90, 0, 0x00, 0x00, 0x00, 0xFF};

#endif