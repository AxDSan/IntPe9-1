#ifndef WINSOCK_H
#define WINSOCK_H

#include <Skeleton.h>

class Winsock : public Skeleton
{
public:
	Winsock();

	void initialize();
	void finalize();
	char *getName();
};

extern Winsock *winsock;

#endif