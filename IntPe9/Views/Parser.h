#ifndef PYTHON_H
#define PYTHON_H


#include <QtGui/QDialog>
#include "ui_Parser.h"

class ParserGui : public QDialog
{
	Q_OBJECT

public:
	ParserGui(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ParserGui();

	Ui::parserView *getView();

public slots:
	void slotShow();
	void slotHide();

private:
	Ui::parserView _parserView;
};

#endif // ABOUT_H
