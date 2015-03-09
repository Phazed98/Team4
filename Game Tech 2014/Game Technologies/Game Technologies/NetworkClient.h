#pragma once
// Networking libraries
#include <winsock2.h>
#include <Windows.h>
#include "NetworkServices.h"
#include <ws2tcpip.h>
#include <stdio.h> 
#include <iostream>
#include "NetworkData.h"

// size of our buffer
#define DEFAULT_BUFLEN 512

// port to connect sockets through 
#define DEFAULT_PORT "6881"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

using namespace std;

class NetworkClient
{

public:

	// for error checking function calls in Winsock library
	int iResult;

	// socket for client to connect to server
	SOCKET ConnectSocket;

	int receivePackets(char *);


	// ctor/dtor
	NetworkClient(void);
	~NetworkClient(void);
};