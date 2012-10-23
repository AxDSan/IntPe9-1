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
#include "PythonWrapper.h"
#include "Stollmann.h"

PythonWrapper *PythonWrapper::instance = NULL;

PythonWrapper::PythonWrapper()
{

}

PythonWrapper::~PythonWrapper()
{

}

PythonWrapper *PythonWrapper::getInstance()
{
	if(PythonWrapper::instance == NULL)
		return PythonWrapper::instance = new PythonWrapper();
	else
		return PythonWrapper::instance;
}

uint8 *PythonWrapper::listToChars(boost::python::list &pList, uint32 *size, bool useMalloc)
{
	boost::python::stl_input_iterator<uint8> begin(pList), end;
	std::list<uint8>bytes;
	bytes.assign(begin, end);

	stollmann->DbgPrint("Got a list to chars with size: %i", bytes.size());
	*size = bytes.size();
	uint8 *	buffer = new uint8[bytes.size()];

	uint32 i = 0;
	while(!bytes.empty())
	{
		buffer[i] = bytes.front();
		bytes.pop_front(); i++;
	}
	return buffer;
}

void PythonWrapper::send(boost::python::list &bytes)
{
	uint32 length;
	uint8 *packet = listToChars(bytes, &length);
	stollmann->pythonComWrite(packet, length);
	delete []packet;
}

void PythonWrapper::write(std::string const&msg)
{
	if(msg.size() <= 0 || msg[0] == 0x0a)
		return;

	stollmann->DbgPrint(msg.c_str());
}