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
#ifndef LOL_SIGNATURE_H
#define LOL_SIGNATURE_H

/**
League of Legends core module for LoL 0.0.0.206
----------------------------------------------------------
 Some notes on how to simple find updates in functions (this method working since version 0.0.0.146)

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

//SendPacket (char __thiscall sendPacket(NetClient *this, size_t length, const void *data, unsigned __int8 channel, int type)) (55 8B EC 83 E4 F8 64 A1 00 00 00 00 6A FF 68 ?? ?? ?? ?? 50 8B 45 14 64 ?? ?? ?? ?? ?? ?? 83)
static unsigned char maskSendPacket[] = "xxxxxxxxxxxxxxx????xxxxx??????x";
static unsigned char signatureSendPacket[] = {0x55, 0x8B, 0xEC, 0x83, 0xE4, 0xF8, 0x64, 0xA1, 0x00, 0x00, 0x00, 0x00, 0x6A, 0xFF, 0x68, 0x0, 0x0, 0x0, 0x0, 0x50, 0x8B, 0x45, 0x14, 0x64, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x83};

//RecvPacket ESP+1C = ENetEvent* (8B 43 08 80 38 FF 75 ?? 33 C9 51)
static unsigned int nopRecvPacket = 2;
static unsigned char maskRecvPacket[] = "xxxxxx?xx";
static unsigned char signatureRecvPacket[] = {0x8B, 0x45, 0x08, 0x85, 0xC0, 0x74, 0x0, 0x83, 0xBF};

//AddEvent hook (ENetEvent *__userpurge addEvent<eax>(struct_a1 *a1<esi>, ENetEvent *a2)) (8B 46 10 83 C0 01 39 46 08 77 ?? E8)
static unsigned char maskAddEvent[] = "xxxxxxxxxx?x";
static unsigned char signatureAddEvent[] = {0x8B, 0x46, 0x10, 0x83, 0xC0, 0x01, 0x39, 0x46, 0x08, 0x77, 0, 0xE8};

//Custom enet malloc function, its thread safe! (void *__cdecl enetMalloc(size_t Size)) (51 56 8D 44 24 ?? 50 B8)
static unsigned char maskEnetMalloc[] = "xxxxx?xx";
static unsigned char signatureEnetMalloc[] = {0x51, 0x56, 0x8D, 0x44, 0x24, 0x0, 0x50, 0xB8};

//Meastro cleanup, always called when you kill LoL or shutdown (int __thiscall maestroCleanup(void *this)) (51 8B 0D ?? ?? ?? ?? 8B 01 8B 90 ?? 00 00 00 FF)
static unsigned char maskMaestroCleanup[] = "xxx????xxxx?xxxx";
static unsigned char signatureMaestroCleanup[] = {0x51, 0x8B, 0x0D, 0, 0, 0, 0, 0x8B, 0x01, 0x8B, 0x90, 0, 0x00, 0x00, 0x00, 0xFF};

#endif