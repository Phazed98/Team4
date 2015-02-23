#pragma once;

#include "NetworkCommon.h"

WSAData wsaData;
struct addrinfo * addr;

class NetworkServer
{
public:
	int init();
	int addressing();
	int sendRecv();
protected:
private:

};