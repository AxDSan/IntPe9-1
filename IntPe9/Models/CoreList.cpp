#include "CoreList.h"

CoreList::CoreList(Cores *cores)
{
	_cores = cores;
	_header << "A" << "Name" << "Version" << "Process";
}

int CoreList::columnCount(const QModelIndex &parent) const
{
	return _header.count();
}


int CoreList::rowCount(const QModelIndex &parent) const
{
	return _cores->count();
}

QVariant CoreList::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.row() >= _cores->count() || index.column() > _header.count() || (role != Qt::DisplayRole && role != Qt::DecorationRole) )
		return QVariant();
	return _cores->at(index.row())->getField(index.column());
}

QVariant CoreList::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
		return QVariant();
	return _header.at(section);
}

void CoreList::refresh()
{
	emit layoutChanged();
}