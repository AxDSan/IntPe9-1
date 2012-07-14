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
#include "Parser.h"

ParserGui::ParserGui(QWidget *parent, Qt::WFlags flags)
: QDialog(parent, flags)
{
	_parserView.setupUi(this);
	_activeSniffer = NULL;
	connect(_parserView.run, SIGNAL(clicked()), this, SLOT(sendCode()));
}

ParserGui::~ParserGui()
{

}

void ParserGui::sendCode()
{
	if(_activeSniffer == NULL)
		return;

	CommandControll *command = (CommandControll*)new uint8[sizeof(CommandControll)+_parserView.input->toPlainText().size()];
	command->cmd = PYTHON;
	command->length = _parserView.input->toPlainText().size();
	memcpy(command->getData(), _parserView.input->toPlainText().toStdString().c_str(), command->length+1);
	_activeSniffer->sendCommand(command);
	delete [](uint8*)command;
}

void ParserGui::setSniffer(Sniffer *sniffer)
{
	if(sniffer == NULL)
		return;

	_activeSniffer = sniffer;
}