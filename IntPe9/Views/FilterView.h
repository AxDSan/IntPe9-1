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

private:
	Filter::SearchMode getFilterMode();

	Filter *_activeFilter;
	Sniffer *_activeSniffer;
	Ui::filterView ui;
};

#endif // ABOUT_H
