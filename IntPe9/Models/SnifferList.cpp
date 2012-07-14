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
#include "SnifferList.h"

SnifferList::SnifferList(Sniffers *sniffers, Sniffer **activeSniffer)
{
	_sniffers = sniffers;
	_activeSniffer = activeSniffer;

	_header << "S" << "R" << "Pid" << "Packets" << "Process";
}

int SnifferList::columnCount(const QModelIndex &parent) const
{
	return _header.count();
}


int SnifferList::rowCount(const QModelIndex &parent) const
{
	return _sniffers->count();
}

QVariant SnifferList::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.row() >= _sniffers->count() || index.column() > _header.count() || (role != Qt::DisplayRole && role != Qt::DecorationRole) )
		return QVariant();
	if(index.column() == 0)
	{
		if(*_activeSniffer == _sniffers->at(index.row()))
			return ">";
		else
			return QVariant();
	}
	return _sniffers->at(index.row())->getField(index.column()-1);
}

QVariant SnifferList::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
		return QVariant();
	return _header.at(section);
}

void SnifferList::refresh()
{
	emit layoutChanged();
}