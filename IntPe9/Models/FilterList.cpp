#include "FilterList.h"

FilterList::FilterList(PacketList *packetList)
{
	_header << "" << "Name" << "Type" << "Hits" << "Search";
	_packetList = packetList;

	_defaultHideAll = false;

	connect(this, SIGNAL(layoutChanged()), this, SLOT(applyFilters()));
}

void FilterList::clear()
{
	Filter *filter;
	foreach(filter, _filters)
		delete filter;
	_filters.clear();
}

FilterList::~FilterList()
{
	clear();
}

int FilterList::columnCount(const QModelIndex &parent) const
{
	return _header.count();
}

int FilterList::rowCount(const QModelIndex &parent) const
{
	return _filters.count();
}

Qt::ItemFlags FilterList::flags(const QModelIndex &index) const
{
	if(index.column() == 0)
		return Qt::ItemIsUserCheckable;
	else
		return Qt::NoItemFlags;
}

QVariant FilterList::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.row() >= _filters.count() || index.column() > _header.count() || (role != Qt::DisplayRole && role != Qt::DecorationRole) )
		return QVariant();
	return _filters.at(index.row())->getField(index.column()-1);
}

QVariant FilterList::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
		return QVariant();
	return _header.at(section);
}

bool FilterList::isInFilter(Packet *packet)
{
	Filter *filter;
	foreach(filter, _filters)
		if(filter->isHit(packet->getData()))
			return true;
	return false;
}

void FilterList::setDefaultHide(bool hideAll)
{
	_defaultHideAll = hideAll;
}

bool FilterList::showInTable(Packet *packet)
{
	/* Truth table for inFilter (XNOR)
	|Filter	|Default| Show	|
	| 0	| 0	| 1	|
	| 0	| 1	| 0	|
	| 1	| 0	| 0	|
	| 1	| 1	| 1	|
	*/
	return !(isInFilter(packet) ^ _defaultHideAll);
}

void FilterList::applyFilters()
{
	QVector<Packet*> *packets = _packetList->getPacketList();
	QVector<Packet*> filtered;

	Packet *packet;
	foreach(packet, *packets)
		if(showInTable(packet))
			filtered.push_back(packet);
	_packetList->setFilteredList(&filtered);	
}

Filter *FilterList::getFilterAt(int row)
{
	return _filters.at(row);
}

void FilterList::addFilter(QString name, Filter::SearchMode mode, QByteArray *search)
{
	addFilter(new Filter(name, mode, search));
}

void FilterList::addFilter(Filter *filter)
{
	_filters.push_back(filter);
}

void FilterList::refresh()
{
	emit layoutChanged();
}