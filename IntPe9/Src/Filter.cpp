#include "Filter.h"

Filter::Filter(QString name, SearchMode mode, QByteArray *search)
{
	_name = name;
	_mode = mode;
	_search = search; //Implicit copy, so we should take care of deleting the object
	_isEnabled = true;
}

Filter::~Filter()
{
	delete _search;
}

bool Filter::isEnabled()
{
	return _isEnabled;
}

void Filter::setEnabled(bool state)
{
	_isEnabled = state;
}

QVariant Filter::getField(int column)
{
	switch(column)
	{
		case 0:
			return _name;
		case 1:
			return getMode();
		case 2:
			return Packet::toHexString(_search);
		default:
			return QVariant();
	}

}

QString Filter::getMode()
{
	switch(_mode)
	{
		case FRONT:
			return "Starts with";
		case ANYWHERE:
			return "Anywhere";
		case END:
			return "Ends with";
		default:
			return "Unknown";
	}
}

bool Filter::isHit(QByteArray *packet)
{
	switch(_mode)
	{
		case FRONT:
			return packet->startsWith(*_search);
		case ANYWHERE:
			return packet->contains(*_search);
		case END:
			return packet->endsWith(*_search);
		default:
			return false;
	}
}