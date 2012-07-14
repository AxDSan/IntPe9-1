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
#ifndef FILTER_H
#define FILTER_H

#include <Common.h>

#include <QObject>
#include <QSettings>
#include <QByteArray>

#include "Packet.h"

class Filter : public QObject
{
	Q_OBJECT

	public:
		enum SearchMode
		{
			FRONT,
			END,
			ANYWHERE
		};

		Filter(QString name, SearchMode mode, QByteArray *search);
		~Filter();

		bool isHit(QByteArray *packet);
		QVariant getField(int column);
		QString getMode();

		bool isEnabled();
		void setEnabled(bool state);

		QString _name;
		QByteArray *_search;
		SearchMode _mode;

	private:
		bool _isEnabled;

};

#endif