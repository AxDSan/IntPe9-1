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
#ifndef FILTER_VIEW_H
#define FILTER_VIEW_H

#include <QtGui/QDialog>

//Classes
#include "Filter.h"
#include "Sniffer.h"
#include "Models/FilterList.h"

//Ui
#include "ui_Filter.h"

class FilterView : public QDialog
{
	Q_OBJECT

public:
	FilterView(QWidget *parent = 0, Qt::WFlags flags = 0);
	~FilterView();

	

public slots:
	void clear();
	void setSniffer(Sniffer *sniffer);
	void processOk();
	void showEmpty();
	void editFilter(const QModelIndex &index);
	void setFilterMode(Filter::SearchMode mode);

private:
	Filter::SearchMode getFilterMode();

	Filter *_activeFilter;
	Sniffer *_activeSniffer;
	Ui::filterView ui;
};

#endif // ABOUT_H
