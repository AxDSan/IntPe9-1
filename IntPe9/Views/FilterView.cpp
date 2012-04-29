#include "FilterView.h"

FilterView::FilterView(QWidget *parent, Qt::WFlags flags)
: QDialog(parent, flags)
{
	ui.setupUi(this);
	connect(ui.buttonSave, SIGNAL(clicked()), this, SLOT(processOk()));
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

Filter::SearchMode FilterView::getFilterMode()
{
	if(ui.radioBegin->isChecked())
		return Filter::FRONT;
	else if(ui.radioEnd->isChecked())
		return Filter::END;
	else
		return Filter::ANYWHERE;
}

void FilterView::processOk()
{
	//We got an ok, so check if we have an active Filter or we making a new one
	if(_activeSniffer == NULL)
		return;

	FilterList *filterList = _activeSniffer->getFilterList();
	if(_activeFilter != NULL)
	{
		//New filter!
		filterList->addFilter(ui.textName->text(), getFilterMode(), Packet::fromHexString(ui.textMatch->toPlainText()));
	}
	else
	{
		//Edit filter!

	}
	_activeSniffer->getFilterList()->refresh();
}

void FilterView::showEmpty()
{
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