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
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <Common.h>
#include <QFile>
#include <QMap>
#include <QDomDocument>

class Protocol
{
	enum Type
	{
		TINT8,
		TINT16,
		TINT32,
		TINT64,
		TUINT8,
		TUINT16,
		TUINT32,
		TUINT64,
		TFLOAT,
	};

	struct Field
	{
		QString name;
		Type type;
		bool isVisible;
		bool isAbsolute;
		uint32 offsetAbsolute;
		QString offsetRelative;
		uint32 length;
	};

	struct Packet
	{
		QString name;
		QList<Field> fields;

		// Identification
		uint32 offset;
		uint32 length;
		uint32 value;
		QByteArray values;
	};

	struct Prot
	{
		QString name;
		QString id;
		QString version;
		QString protocolVersion;
		QList<Packet*> packets;
	};

	public:
		Protocol(const char *xml);
		
	private:
		QDomDocument *dom;
		QMap<QString, Prot> protocols;
		QMap<QString, Type> typeMap;

		// Helper functions
		uint32 getLength(Type type);
		Type strToType(QString type);
};

#endif