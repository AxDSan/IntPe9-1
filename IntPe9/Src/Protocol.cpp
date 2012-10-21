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
#include "Protocol.h"

Protocol::Protocol(const char *xml)
{
	dom = new QDomDocument();
	QFile file(xml);
	if (file.open(QIODevice::ReadOnly))
	{
		if(dom->setContent(&file))
		{
			QDomElement xProtocols = dom->documentElement();
			
			// Parse all protocols in this database
			while(!xProtocols.isNull())
			{
				Prot protocol;
				protocol.name = xProtocols.attribute("name");
				protocol.id = xProtocols.attribute("id");
				protocol.version = xProtocols.attribute("version");
				protocol.protocolVersion = xProtocols.attribute("pversion");				
				protocols.insert(protocol.id, protocol);

				// Get reference and start parsing all packets
				Prot *prot = &protocols[protocol.id];

				QDomElement xPacket = xProtocols.firstChildElement();
				while(!xPacket.isNull())
				{
					Packet *packet = new Packet();
					packet->name = xPacket.attribute("name");
					packet->offset = xPacket.attribute("offset").toInt();
					packet->length = xPacket.attribute("length").toInt();
					packet->value = xPacket.attribute("value").toInt();
					prot->packets.push_back(packet);
					

					// Get reference and start parsing all packets
					QDomElement xFields = xPacket.firstChildElement();
					while(!xFields.isNull())
					{
						Field field;
						field.type = strToType(xFields.attribute("type"));
						field.name = xFields.text();
						field.length = (xFields.hasAttribute("length") ? xFields.attribute("type").toInt() : getLength(field.type));
						field.isVisible = (xFields.attribute("visible", "true") == "true");

						
						field.offsetAbsolute = xFields.attribute("offset").toInt(&field.isAbsolute);
						if(!field.isAbsolute)
							field.offsetRelative = xFields.attribute("offset");

						xFields = xPacket.nextSiblingElement();
					}
					xPacket = xPacket.nextSiblingElement();
				}
				xProtocols = xProtocols.nextSiblingElement();
			}
		}
		else
			file.close();
	}
}

Protocol::Type Protocol::strToType(QString type)
{
	if(typeMap.empty())
	{
		// Build table cache
		typeMap.insert("int8", TINT8);
		typeMap.insert("int16", TINT16);
		typeMap.insert("int32", TINT32);
		typeMap.insert("int64", TINT64);
		typeMap.insert("uint8", TUINT8);
		typeMap.insert("uint16", TUINT16);
		typeMap.insert("uint32", TUINT32);
		typeMap.insert("uint64", TUINT64);
		typeMap.insert("float", TFLOAT);
	}
	return typeMap[type];
}

uint32 Protocol::getLength(Type type)
{
	switch(type)
	{
		case TINT8: return sizeof(int8);
		case TINT16: return sizeof(int16);
		case TINT32: return sizeof(int32);
		case TINT64: return sizeof(int64);
		case TUINT8: return sizeof(uint8);
		case TUINT16: return sizeof(uint16);
		case TUINT32: return sizeof(uint32);
		case TUINT64: return sizeof(uint64);
		case TFLOAT: return sizeof(float);
	}
	return 0;
}