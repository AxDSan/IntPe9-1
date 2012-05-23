#ifndef FILTER_LIST_H
#define FILTER_LIST_H

#include <Common.h>
#include <QAbstractListModel>
#include <QVector>
#include <QStringList>

#include "Filter.h"
#include "Models/PacketList.h"

class FilterList : public QAbstractListModel
{
	Q_OBJECT

	public:
		FilterList(PacketList *packetList);
		~FilterList();

		//Standard implementations
		int columnCount(const QModelIndex &parent) const;
		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		QVariant data(const QModelIndex &index, int role) const;
		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
		Qt::ItemFlags flags(const QModelIndex & index) const;

		//New functions
		Filter *getFilterAt(int row);
		bool showInTable(Packet *packet);
		bool isInFilter(Packet *packet);
		void addFilter(QString name, Filter::SearchMode mode, QByteArray *search);
		void addFilter(Filter *filter);

		void clear();
		bool getDefaultHide();

	private:
		QVector<Filter*> _filters;
		QStringList _header;
		PacketList *_packetList;
		bool _defaultHideAll;

	public slots:
		void applyFilters();
		void refresh();
		void setDefaultHide(bool hideAll);
};

#endif