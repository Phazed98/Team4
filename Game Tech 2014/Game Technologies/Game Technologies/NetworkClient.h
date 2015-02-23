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
	bool sendData(vector<messageInfo> mI);
protected:
private:
};