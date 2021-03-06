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
#include "Packet.h"

Packet::Packet(MessagePacket *data)
{
	protocol = NULL;

	//Copy data
	_length = data->length;
	_type = data->type;
	_data = new QByteArray((const char*)data->getData(), data->length);
	_description = QString(data->description);

	//Create list view data
	_summary = toHexString(_data, SUMMARY_LEN);
	if(_data->size() > SUMMARY_LEN)
		_summary.append("... "+toHexString(_data, 2, _data->size()-2));
}

Packet::~Packet()
{
	delete _data;
}

QList<QTreeWidgetItem*> *Packet::getProtocol(Protocol *prot)
{
	// Build the protocol tree
	if(protocol == NULL)
	{
		protocol = new QList<QTreeWidgetItem*>();
		for (int i = 0; i < 10; ++i)
			protocol->append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("item: %1").arg(i))));
	}
	return protocol;
}

QString Packet::toHexString(QByteArray *buffer, int size, int start)
{

	QString format;
	QString hex;

	if(start >= buffer->size())
		return NULL;

	if(size == 0)
		size = buffer->size();

	if(size+start > buffer->size())
		size = buffer->size() - start;

	for(int i = start; i < size+start; i++)
		format = format % hex.sprintf("%02X ", (uint8)buffer->at(i));

	if(buffer->size() >= 1)
		format.chop(1); //Remove last space

	return format;
}

QByteArray *Packet::fromHexString(QString string)
{
	QString stripped = string.remove(QRegExp("[^a-zA-Z0-9]")); //Removes everything except a-z (and capital) and 0-9

	//Error, its not an even numbered string (so not in correct form)
	if(stripped.size() % 2 != 0)
		return NULL;

	QByteArray *buffer = new QByteArray(stripped.size() % 2, '\0');
	bool good;
	for(int i = 0; i < stripped.size(); i+=2)
	{
		QStringRef hex(&stripped, i, 2);
		uint8 c = (uint8)hex.toString().toUShort(&good, 16);
		if(!good)
		{
			delete buffer;
			return NULL;
		}

		buffer->push_back(c);
	}
	return buffer;
}

QString Packet::toC()
{
	if(_data->size() <= 0)
		return "{}";
	QString out, format;
	uint8 hex;

	out = format.sprintf("{0x%02X", (uint8)_data->at(0));
	for(uint32 i = 1; i < _data->size(); i++)
		out = out % format.sprintf(", 0x%02X", (uint8)_data->at(i));
	out += "};";

	return out;
}

QString Packet::toPython()
{
	if(_data->size() <= 0)
		return "[]";
	QString out, format;
	uint8 hex;

	out = format.sprintf("[0x%02X", (uint8)_data->at(0));
	for(uint32 i = 1; i < _data->size(); i++)
		out = out % format.sprintf(", 0x%02X", (uint8)_data->at(i));
	out += "]";

	return out;
}

QString Packet::strInfoHeader()
{
	QString out;
	out += "Packet: ";
		if(_type == WSASENDTO || _type == WSASEND)
			out += "C -> S";
		else
			out += "S -> C";

	out += ", Length: " + QString::number(_length);
	out += ", Description: " + _description + "\n";

	return out;
}

QString Packet::strFullDump()
{
	QString out;
	QString format;
	for(uint32 s = 0, x = 0, c = 0; s < _length; s+=16)
	{
		//Format line number
		out += format.sprintf("%08x     ", s);

		//Format hex
		for(x = s, c = 0; x < _length && c < 16; x++, c++)
			out += format.sprintf("%02X ", (uint8)_data->at(x));

		//Pad remaining space
		for(; c < 16; c++)
			out += "   ";
		out += "    "; 

		//Format ascii
		for(x = s, c = 0; x < _length && c < 16; x++, c++)
			out += (isprint(_data->at(x)) ? _data->at(x) : '.');
		out += "\n";

		//Break if we are done
		if(s+c >= _length)
			break;
	}
	return out;
}

QVariant Packet::getField(int column)
{
	switch(column)
	{
		case 0:
			return getIcon();
		case 1:
			return getLength();
		case 2:
			return getBody();
		case 3:
			return _description;
		default:
			return QVariant();
	}
}

QPixmap Packet::getIcon()
{
	if(_type == SEND)
		return QPixmap(":/Common/out3.png");
	else if(_type == WSASEND)
		return QPixmap(":/Common/out.png");
	else if(_type == WSASENDTO || _type == SENDTO)
		return QPixmap(":/Common/out2.png");
	else if(_type == RECV)
		return QPixmap(":/Common/in3.png");
	else if(_type == WSARECV)
		return QPixmap(":/Common/in.png");
	else if(_type == WSARECVFROM || _type == RECVFROM)
		return QPixmap(":/Common/in2.png");
	else if(_type == INJECT_RECV)
		return QPixmap(":/Common/customIn.png");
	else if(_type == INJECT_SEND)
		return QPixmap(":/Common/customOut.png");
	else
		return QPixmap();
}	

QVariant Packet::getBody()
{
	return _summary;
}

PacketType Packet::getType()
{
	return _type;
}

uint32 Packet::getLength()
{
	return _length;
}

QByteArray *Packet::getData()
{
	return _data;
}