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