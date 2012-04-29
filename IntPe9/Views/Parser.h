#ifndef PYTHON_H
#define PYTHON_H

#include <QtGui/QDialog>

//Classes
#include "Sniffer.h"

//Ui
#include "ui_Parser.h"

class ParserGui : public QDialog
{
	Q_OBJECT

public:
	ParserGui(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ParserGui();


public slots:
	void sendCode();
	void setSniffer(Sniffer *sniffer);

private:
	Sniffer *_activeSniffer;
	Ui::parserView _parserView;
};

#endif // ABOUT_H
