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
