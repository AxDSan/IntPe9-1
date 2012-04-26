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
	void sendPacket(boost::python::list &bytes, uint8 channel, uint32 flags = 1);
	void recvPacket(boost::python::list &bytes, uint8 channel, uint32 flags = 1);
	void sendChat(uint32 type, std::string const&msg);

	//Variables
	static PythonWrapper *instance;
private:
	uint8 *listToChars(boost::python::list &pList, uint32 *size, bool useMalloc = false);
};

#endif