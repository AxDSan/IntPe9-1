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