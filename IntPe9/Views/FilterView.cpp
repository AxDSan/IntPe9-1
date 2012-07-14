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
#include "FilterView.h"

FilterView::FilterView(QWidget *parent, Qt::WFlags flags)
: QDialog(parent, flags)
{
	ui.setupUi(this);
	connect(ui.buttonSave, SIGNAL(clicked()), this, SLOT(processOk()));
	_activeFilter = NULL;
}

FilterView::~FilterView()
{

}

void FilterView::clear()
{
	ui.textMatch->clear();
	ui.textName->clear();
	ui.radioAnywhere->setChecked(true);
}

void FilterView::setFilterMode(Filter::SearchMode mode)
{
	if(mode == Filter::FRONT)
		ui.radioBegin->setChecked(true);
	else if(mode == Filter::END)
		ui.radioEnd->setChecked(true);
	else
		ui.radioAnywhere->setChecked(true);
}

Filter::SearchMode FilterView::getFilterMode()
{
	if(ui.radioBegin->isChecked())
		return Filter::FRONT;
	else if(ui.radioEnd->isChecked())
		return Filter::END;
	else
		return Filter::ANYWHERE;
}

void FilterView::editFilter(const QModelIndex &current)
{
	FilterList *model = (FilterList*)current.model();
	_activeFilter = model->getFilterAt(current.row());

	//Set the fields
	ui.textName->setText(_activeFilter->_name);
	setFilterMode(_activeFilter->_mode);
	ui.textMatch->setPlainText(Packet::toHexString(_activeFilter->_search));

	this->show();
}

void FilterView::processOk()
{
	//We got an ok, so check if we have an active Filter or we making a new one
	if(_activeSniffer == NULL)
		return;

	FilterList *filterList = _activeSniffer->getFilterList();
	if(_activeFilter == NULL)
	{
		//New filter!
		filterList->addFilter(ui.textName->text(), getFilterMode(), Packet::fromHexString(ui.textMatch->toPlainText()));
	}
	else
	{
		//Edit filter!
		_activeFilter->_name = ui.textName->text();
		_activeFilter->_mode = getFilterMode();
		_activeFilter->_search =  Packet::fromHexString(ui.textMatch->toPlainText());
		_activeFilter = NULL;
	}
	_activeSniffer->getFilterList()->refresh();
	this->hide();
}

void FilterView::showEmpty()
{
	_activeFilter = NULL;
	clear();
	this->show();
}

void FilterView::setSniffer(Sniffer *sniffer)
{
	if(sniffer == NULL)
		return;

	clear();
	_activeSniffer = sniffer;
}