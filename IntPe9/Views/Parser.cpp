#include "Parser.h"

ParserGui::ParserGui(QWidget *parent, Qt::WFlags flags)
: QDialog(parent, flags)
{
	_parserView.setupUi(this);
	connect(_parserView.run, SIGNAL(clicked()), this, SLOT(sendCode()));
}

ParserGui::~ParserGui()
{

}

void ParserGui::sendCode()
{
	CommandControll *command = (CommandControll*)new uint8[sizeof(CommandControll)+_parserView.input->toPlainText().size()];
	command->cmd = PYTHON;
	command->length = _parserView.input->toPlainText().size();
	memcpy(command->getData(), _parserView.input->toPlainText().toStdString().c_str(), command->length+1);
	_activeSniffer->sendCommand(command);
	delete [](uint8*)command;
}

void ParserGui::showAndActivate(Sniffer *sniffer)
{
	if(sniffer == NULL)
		return;

	_activeSniffer = sniffer;
	this->show();
}