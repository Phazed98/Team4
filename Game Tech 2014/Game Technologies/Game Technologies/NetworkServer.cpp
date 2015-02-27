#include "NetworkServer.h"
#include "../../nclgl/Matrix4.h"

NetworkServer::NetworkServer(void)
{
	// create WSADATA object
	WSADATA wsaData;

	// our sockets for the server
	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;

	// address info for the server to listen to
	struct addrinfo *result = NULL;
	struct addrinfo hints;


	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		cout << "WSAStartup failed with error: " << iResult << endl;
	}

	// set address information
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;    // TCP connection!!!
	hints.ai_flags = AI_PASSIVE;


	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);

	if (iResult != 0)
	{
		cout << "getaddrinfo failed with error: " << iResult << endl;
		WSACleanup();
	}


	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET)
	{
		cout << "socket failed with error: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
	}

	// Set the mode of the socket to be nonblocking
	u_long iMode = 1;
	iResult = ioctlsocket(ListenSocket, FIONBIO, &iMode);

	if (iResult == SOCKET_ERROR)
	{
		cout << "ioctlsocket failed with error: " << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
	}
	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);

	if (iResult == SOCKET_ERROR)
	{
		cout << "bind failed with error: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
	}

	// no longer need address information
	freeaddrinfo(result);

	// start listening for new clients attempting to connect
	iResult = listen(ListenSocket, SOMAXCONN);

	if (iResult == SOCKET_ERROR)
	{
		cout << "listen failed with error: " << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
	}

	cout << "Server Init Success!!" << endl;
}


// accept new connections
bool NetworkServer::acceptNewClient(unsigned int & id)
{
	// if client waiting, accept the connection and save the socket
	ClientSocket = accept(ListenSocket, NULL, NULL);

	if (ClientSocket != INVALID_SOCKET)
	{
		//disable nagle on the client's socket
		char value = 1;
		setsockopt(ClientSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

		// insert new client into session id table
		sessions.insert(pair<unsigned int, SOCKET>(id, ClientSocket));
		return true;
	}

	return false;
}


// receive incoming data
int NetworkServer::receiveData(unsigned int client_id, char * recvbuf)
{
	if (sessions.find(client_id) != sessions.end())
	{
		SOCKET currentSocket = sessions[client_id];
		iResult = NetworkServices::receiveMessage(currentSocket, recvbuf, MAX_PACKET_SIZE);
		if (iResult == 0)
		{
			cout << "Connection closed" << endl;
			closesocket(currentSocket);
		}
		return iResult;
	}
	return 0;
}


// send data to all clients
void NetworkServer::sendToAll(char* packets, int totalSize)
{
	SOCKET currentSocket;
	std::map<unsigned int, SOCKET>::iterator iter;
	int iSendResult;

	for (iter = sessions.begin(); iter != sessions.end(); iter++)
	{
		currentSocket = iter->second;
		iSendResult = NetworkServices::sendMessage(currentSocket, packets, totalSize);

		if (iSendResult == SOCKET_ERROR)
		{
			cout << "send failed with error: " << WSAGetLastError() << endl;
			closesocket(currentSocket);
		}
	}
}


//
//int NetworkServer::init()
//{
//	// call to startup
//	int error = WSAStartup(MAKEWORD(HIGHVERSION, LOWVERSION), &wsaData);
//	if (error != 0)
//	{
//		fprintf(stderr, " WSAStartup failed with error : %d.\n", error);
//		return 1;
//	}
//	// check version values
//
//	if (LOBYTE(wsaData.wVersion) != LOWVERSION || HIBYTE(wsaData.wVersion) != HIGHVERSION)
//	{
//		printf(" The version requested cannot be supported. \n");
//		printf(" Version set is %d.%d\n", LOBYTE(wsaData.wVersion),
//			HIBYTE(wsaData.wVersion));
//		WSACleanup();
//		return 1;
//	}
//	else
//	{
//		printf(" The Winsock API has been successfully initialised. \n"
//			" You are using version %d.%d.\n\n",
//			HIBYTE(wsaData.wVersion),
//			LOBYTE(wsaData.wVersion));
//		return 0;
//	}
//}
//
//
//int NetworkServer::sendMessage(SOCKET curSocket, char * message, int messageSize)
//{
//	return send(curSocket, message, messageSize, 0);
//}
//
//int NetworkServer::recvMessage(SOCKET curSocket, char * buffer, int bufSize)
//{
//	return recv(curSocket, buffer, bufSize, 0);
//}
//
//
//int NetworkServer::addressing()
//{
//	int result;
//	struct addrinfo hints;
//	struct addrinfo * temp;
//
//	// initialise the hints struct
//	memset(&hints, 0, sizeof (hints));
//	hints.ai_family = FAMILY;
//	hints.ai_socktype = TYPE;
//	hints.ai_flags = FLAGS;
//	hints.ai_protocol = PROTOCOL;
//
//	result = getaddrinfo(HOST, PORT, &hints, &addr);
//
//	if (result != 0)
//	{
//		printf(" getaddrinfo () failed with error : \n%d: %s\n", result, gai_strerror(WSAGetLastError()));
//		WSACleanup();
//		return 1;
//	}
//
//	// need to walk the linked list
//	printf(" Addressing information :\n");
//
//	int i = 0;
//	for (temp = addr; temp != NULL; temp = temp->ai_next)
//	{
//		printf("\n Entry %d:\n", ++i);
//		switch (temp->ai_family)
//		{
//		case AF_INET:
//			printf("\t Address family : AF_INET \n");
//			break;
//		case AF_INET6:
//			printf("\t Address family : AF_INET6 \n");
//			break;
//		}
//		switch (temp->ai_protocol)
//		{
//		case IPPROTO_TCP:
//			printf("\t Protocol : TCP \n");
//			break;
//		case IPPROTO_UDP:
//			printf("\t Protocol : UDP \n");
//			break;
//		}
//		switch (temp->ai_socktype)
//		{
//		case SOCK_STREAM:
//			printf("\t Socket type : Stream \n");
//			break;
//		case SOCK_DGRAM:
//			printf("\t Socket type : Datagram \n");
//			break;
//		}
//	}
//	return 0;
//}
//
//
//char buffer[2048];
//char buff[BUFFSIZE];
//int NetworkServer::sendRecv(void)
//{
//	SOCKET s = NULL;
//	// initialise socket library
//	if (init())
//	{
//		printf(" Unable to initialise the Winsock library \n");
//		exit(1);
//	}
//
//	// intiailise addressing information
//	if (addressing() != 0)
//	{
//		printf(" Uanble to initialise addressing information \n");
//		exit(1);
//	}
//	// create a socket for the server to listen on
//	if ((s = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) == INVALID_SOCKET)
//	{
//		printf(" Unable to create a socket \n");
//		printf(" Failed with error : %d\n%s\n", WSAGetLastError(), gai_strerror(WSAGetLastError()));
//		exit(1);
//	}
//	else
//	{
//		printf("\n Socket created successfully. \n");
//	}
//
//	// bind to the socket created above
//	if (bind(s, addr->ai_addr, addr->ai_addrlen) != 0)
//	{
//		printf(" Unable to bind to socket \n");
//		printf(" Failed with error : %d\n%s\n", WSAGetLastError(), gai_strerror(WSAGetLastError()));
//	}
//	else
//	{
//		printf(" Bound to socket. \n");
//	}
//	// finished with addrinfo struct now
//
//	freeaddrinfo(addr);
//	// listen on the socket
//	if (listen(s, BACKLOG) != 0)
//	{
//		printf(" Unable to listen on socket \n");
//		printf(" Failed with error : %d\n%s\n", WSAGetLastError(), gai_strerror(WSAGetLastError()));
//	}
//	else
//	{
//		printf(" Listening on the socket. \n");
//	}
//
//	// continually accept new connections
//	while (GameClass::GetGameClass().getCurrentState()!= GAME_EXIT)
//	{
//		cout << GameClass::GetGameClass().getCurrentState() << endl;
//		printf("\n Waiting for connections ... \n");
//		SOCKET inc = NULL;
//		struct sockaddr_storage inc_addr;
//		socklen_t inc_size = sizeof (inc_addr);
//		// accept new connection from a client
//		if ((inc = accept(s, (struct sockaddr *) & inc_addr, &inc_size)) == INVALID_SOCKET)
//		{
//			printf(" Unable to accept new connection \n");
//			printf(" Failed with error : %d\n%s\n", WSAGetLastError(), gai_strerror(WSAGetLastError()));
//		}
//		else
//		{
//			printf(" Accepted a new connection ...\n");
//		}
//
//		// send message to the client
//		char * hw = " Hello Client ";
//		send(inc, hw, strlen(hw), 0);
//
//		// receive message from client
//		int bytesreceived;
//
//		int int_data;
//		messageInfo info_data;
//		Matrix4 mat_data;
//
//		vector<messageInfo> message;
//
//		if ((bytesreceived = recv(inc, buffer, BUFFSIZE - 1, 0)) == -1)
//		{
//			printf(" Error receiving \n");
//			printf(" Failed with error : %d\n%s\n", WSAGetLastError(), gai_strerror(WSAGetLastError()));
//		}
//		else
//		{
//			message.resize(bytesreceived / sizeof(messageInfo));
//			memcpy(&message[0], buffer, bytesreceived);
//
//			std::cout << " Server Message Recieved. " << std::endl;
//			std::cout << " Recieved " << bytesreceived << " Bytes." << std::endl;
//			for (messageInfo s : message)
//			{
//				std::cout << s.objectID << ": "<< s.Position << std::endl;
//
//				vector<GameEntity*>  entityVector = GameClass::GetGameClass().getAllentitities();
//				for (GameEntity* entity : entityVector)
//				{
//					if (entity->objectID == s.objectID)
//					{
//						//entity->GetPhysicsNode().SetPosition(s.Position);
//						//entity->GetPhysicsNode().SetOrientation(s.Orientation);
//					}
//				}
//			}
//			
//		}
//
//		closesocket(inc);
//
//	}
//	closesocket(s);
//	WSACleanup();
//}