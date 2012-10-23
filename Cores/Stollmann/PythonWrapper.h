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
#ifndef PYTHON_WRAPPER_H
#define PYTHON_WRAPPER_H

#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>

#include <Common.h>
#include <string>

using std::string;

class PythonWrapper
{
public:
	PythonWrapper();
	~PythonWrapper();
	static PythonWrapper *getInstance();

	//Published functions
	void write(std::string const&msg);
	void send(boost::python::list &bytes);

	//Variables
	static PythonWrapper *instance;
private:
	uint8 *listToChars(boost::python::list &pList, uint32 *size, bool useMalloc = false);
};
#endif