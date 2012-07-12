#include "CoreList.h"

CoreList::CoreList(Cores *cores)
{
	_cores = cores;
	_header << "" << "A" << "P" << "Name" << "V" << "Process";
}

int CoreList::columnCount(const QModelIndex &parent) const
{
	return _header.count();
}


int CoreList::rowCount(const QModelIndex &parent) const
{
	return _cores->count();
}

Qt::ItemFlags CoreList::flags(const QModelIndex &index) const
{
	if(index.column() == 0)
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
	else
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool CoreList::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if(role == Qt::CheckStateRole && index.column() == 0)
	{
		_cores->at(index.row())->setEnabled(value.toBool());
		emit dataChanged(index, index);
		emit layoutChanged();
		return true;
	}
	return false;
}

QVariant CoreList::data(const QModelIndex &index, int role) const
{
	if(index.column() == 0 && role == Qt::CheckStateRole)
		return (_cores->at(index.row())->isEnabled()) ? Qt::Checked : Qt::Unchecked;
	if (!index.isValid() || index.row() >= _cores->count() || index.column() > _header.count() || (role != Qt::DisplayRole && role != Qt::DecorationRole) )
		return QVariant();
	return _cores->at(index.row())->getField(index.column()-1);
}

QVariant CoreList::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
		return QVariant();
	return _header.at(section);
}

Core *CoreList::getCoreAt(int row)
{
	return _cores->at(row);
}

void CoreList::refresh()
{
	emit layoutChanged();
}