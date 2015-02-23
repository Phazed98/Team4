#pragma once;
#include "NetworkCommon.h"


WSAData wsaClientData;
struct addrinfo * clientaddr;


class NetworkClient
{
public:
	int init();
	int addressing();
	int sendRecv();
protected:
private:
};