#include "Parser.h"

ParserGui::ParserGui(QWidget *parent, Qt::WFlags flags)
: QDialog(parent, flags)
{
	_parserView.setupUi(this);
}

ParserGui::~ParserGui()
{

}

void ParserGui::slotShow()
{
	this->show();
}

void ParserGui::slotHide()
{
	this->hide();
}

Ui::parserView *ParserGui::getView()
{
	return &_parserView;
}