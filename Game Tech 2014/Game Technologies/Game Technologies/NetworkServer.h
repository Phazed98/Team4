#pragma once
#include <winsock2.h>
#include <Windows.h>
#include "NetworkServices.h"
#include <ws2tcpip.h>
#include <map>
#include <iostream>
#include "NetworkData.h"

using namespace std;
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "6881" 


class NetworkServer
{

public:

	NetworkServer(void);
	~NetworkServer(void);


	// Socket to listen for new connections
	SOCKET ListenSocket;

	// Socket to give to the clients
	SOCKET ClientSocket;

	// for error checking return values
	int iResult;

	// table to keep track of each client's socket
	std::map<unsigned int, SOCKET> sessions;

	bool acceptNewClient(unsigned int & id);


	// receive incoming data
	int receiveData(unsigned int client_id, char * recvbuf);

	// send data to all clients
	void sendToAll(char * packets, int totalSize);


};

//
//
//WSAData wsaData;
//struct addrinfo * addr;
//
//class NetworkServer
//{
//public:
//	NetworkServer(void);
//	~NetworkServer(void);
//
//
//	SOCKET ListenSocket;
//	SOCKET ClientSocket;
//
//	int iResult;
//
//	std::map<unsigned int, SOCKET> sessions;
//
//	// accept new connections
//	bool acceptNewClient(unsigned int & id);
//
//
//	int init();
//	int addressing();
//	int sendRecv();
//	static int sendMessage(SOCKET curSocket, char* message, int messageSize);
//	static int recvMessage(SOCKET curSocket, char* buffer, int bufSize);
//
//
//};