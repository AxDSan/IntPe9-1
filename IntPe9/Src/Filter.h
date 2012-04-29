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

	private:
		QString _name;
		QByteArray *_search;
		SearchMode _mode;
};

#endif