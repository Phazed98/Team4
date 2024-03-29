#include "NetworkClient.h"

NetworkClient::NetworkClient(string ipAddress)
{
	WSADATA wsaData;
	ConnectSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0)
	{
		cout << "WSAStartup failed with error: " << iResult << endl;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(ipAddress.c_str(), DEFAULT_PORT, &hints, &result);

	if (iResult != 0)
	{
		cout << "Getaddrinfo failed with error: " << iResult << endl;
		WSACleanup();
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (ConnectSocket == INVALID_SOCKET)
		{
			cout << "Socket failed with error: " << WSAGetLastError() << endl;
			WSACleanup();
		}

		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

		if (iResult == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			cout << "The server is down... did not connect" << endl;
		}
	}

	//	 no longer need address info for server
	freeaddrinfo(result);

	//	 if connection failed
	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
	}

	//	 Set the mode of the socket to be nonblocking
	u_long iMode = 1;

	iResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
	if (iResult == SOCKET_ERROR)
	{
		cout << "ioctlsocket failed with error: " << WSAGetLastError() << endl;
		closesocket(ConnectSocket);
		WSACleanup();
	}

	//	disable nagle
	char value = 1;
	setsockopt(ConnectSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

	cout << "Client Networking Init Success" << endl;
}


NetworkClient::~NetworkClient(void)
{


}


int NetworkClient::receivePackets(char * recvbuf)
{
	iResult = NetworkServices::receiveMessage(ConnectSocket, recvbuf, MAX_PACKET_SIZE);

	if (iResult == 0)
	{
		printf("Connection closed\n");
		closesocket(ConnectSocket);
		WSACleanup();
		exit(1);
	}

	return iResult;
}




//
//int NetworkClient::init()
//{
//	// call to startup
//	int error = WSAStartup(MAKEWORD(HIGHVERSION, LOWVERSION), &wsaClientData);
//	if (error != 0)
//	{
//		fprintf(stderr, " WSAStartup failed with error : %d.\n", error);
//		return 1;
//	}
//	// check version values
//
//	if (LOBYTE(wsaClientData.wVersion) != LOWVERSION || HIBYTE(wsaClientData.wVersion) != HIGHVERSION)
//	{
//		printf(" The version requested cannot be supported .\n");
//		printf(" Version set is %d.%d\n", LOBYTE(wsaClientData.wVersion), HIBYTE(wsaClientData.wVersion));
//		WSACleanup();
//		return 1;
//	}
//	else
//	{
//		printf(" The Winsock API has been successfully initialised .\n"
//			" You are using version %d.%d.\n\n", HIBYTE(wsaClientData.wVersion), LOBYTE(wsaClientData.wVersion));
//		return 0;
//	}
//}
//
//int NetworkClient::addressing()
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
//	result = getaddrinfo(HOST, PORT, &hints, &clientaddr);
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
//	for (temp = clientaddr; temp != NULL; temp = temp->ai_next)
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
//int NetworkClient::sendRecv(void)
//{
//	SOCKET s = NULL;
//
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
//	if ((s = socket(clientaddr->ai_family, clientaddr->ai_socktype, clientaddr->ai_protocol)) == INVALID_SOCKET)
//	{
//		printf(" Unable to create a socket \n");
//		printf(" Failed with error : %d\n%s\n", WSAGetLastError(), gai_strerror(WSAGetLastError()));
//		exit(1);
//	}
//	else
//	{
//		printf("\n Socket created successfully .\n");
//	}
//
//	// connect to the server
//	if (connect(s, clientaddr->ai_addr, clientaddr->ai_addrlen) != 0)
//	{
//		printf(" Unable to connect to server \n");
//		printf(" Failed with error : %d\n%s\n", WSAGetLastError(), gai_strerror(WSAGetLastError()));
//	}
//	else
//	{
//		printf("\n Connected to the server .\n");
//	}
//
//	// finished with addrinfo struct now
//	freeaddrinfo(clientaddr);
//
//	// accept message from server and close
//	int bytesreceived;
//	char buff[BUFFSIZE];
//
//	if ((bytesreceived = recv(s, buff, BUFFSIZE - 1, 0)) == -1)
//	{
//		printf(" Error receiving \n");
//		printf(" Failed with error : %d\n%s\n", WSAGetLastError(), gai_strerror(WSAGetLastError()));
//	}
//	else
//	{
//		buff[bytesreceived] = '\0';
//		printf("\n\n Client Message received . Received %d bytes. \n Message is: %s", bytesreceived, buff);
//		printf("\n\n");
//	}
//
//
//	int int_data = 191123;
//	messageInfo info_data; 
//	Matrix4 mat_data;
//	mat_data.ToIdentity();
//	if (GameClass::GetGameClass().getCurrentState() == GAME_PLAYING)
//	{
//		
//	}
//	
//
//
//	send(s, (char*)&mat_data, sizeof(messageInfo), 0);
//
//	closesocket(s);
//	WSACleanup();
//}
//
//bool NetworkClient::sendData(vector<messageInfo> messInfo)
//{
//	SOCKET s = NULL;
//
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
//	if ((s = socket(clientaddr->ai_family, clientaddr->ai_socktype, clientaddr->ai_protocol)) == INVALID_SOCKET)
//	{
//		printf(" Unable to create a socket \n");
//		printf(" Failed with error : %d\n%s\n", WSAGetLastError(), gai_strerror(WSAGetLastError()));
//		exit(1);
//	}
//	else
//	{
//		printf("\n Socket created successfully .\n");
//	}
//
//	// connect to the server
//	if (connect(s, clientaddr->ai_addr, clientaddr->ai_addrlen) != 0)
//	{
//		printf(" Unable to connect to server \n");
//		printf(" Failed with error : %d\n%s\n", WSAGetLastError(), gai_strerror(WSAGetLastError()));
//	}
//	else
//	{
//		printf("\n Connected to the server .\n");
//	}
//
//	// finished with addrinfo struct now
//	freeaddrinfo(clientaddr);
//
//	// accept message from server and close
//	int bytesreceived;
//	char buff[BUFFSIZE];
//
//	if ((bytesreceived = recv(s, buff, BUFFSIZE - 1, 0)) == -1)
//	{
//		printf(" Error receiving \n");
//		printf(" Failed with error : %d\n%s\n", WSAGetLastError(), gai_strerror(WSAGetLastError()));
//	}
//	else
//	{
//		buff[bytesreceived] = '\0';
//		printf("\n\n Client Message received . Received %d bytes. \n Message is: %s", bytesreceived, buff);
//		printf("\n\n");
//	}
//
//
//	int int_data = 191123;
//	messageInfo info_data;
//	Matrix4 mat_data;
//	//mat_data.ToIdentity();
//	if (GameClass::GetGameClass().getCurrentState() == GAME_PLAYING)
//	{
//
//	}
//
//	send(s, (char*)&messInfo[0], sizeof(messageInfo)*messInfo.size(), 0);
//
//	closesocket(s);
//	WSACleanup();
//}