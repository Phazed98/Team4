#include "NetworkServer.h"
#include "../../nclgl/Matrix4.h"


int NetworkServer::init()
{
	// call to startup
	int error = WSAStartup(MAKEWORD(HIGHVERSION, LOWVERSION), &wsaData);
	if (error != 0)
	{
		fprintf(stderr, " WSAStartup failed with error : %d.\n", error);
		return 1;
	}
	// check version values

	if (LOBYTE(wsaData.wVersion) != LOWVERSION || HIBYTE(wsaData.wVersion) != HIGHVERSION)
	{
		printf(" The version requested cannot be supported. \n");
		printf(" Version set is %d.%d\n", LOBYTE(wsaData.wVersion),
			HIBYTE(wsaData.wVersion));
		WSACleanup();
		return 1;
	}
	else
	{
		printf(" The Winsock API has been successfully initialised. \n"
			" You are using version %d.%d.\n\n",
			HIBYTE(wsaData.wVersion),
			LOBYTE(wsaData.wVersion));
		return 0;
	}
}


int NetworkServer::addressing()
{
	int result;
	struct addrinfo hints;
	struct addrinfo * temp;

	// initialise the hints struct
	memset(&hints, 0, sizeof (hints));
	hints.ai_family = FAMILY;
	hints.ai_socktype = TYPE;
	hints.ai_flags = FLAGS;
	hints.ai_protocol = PROTOCOL;

	result = getaddrinfo(HOST, PORT, &hints, &addr);

	if (result != 0)
	{
		printf(" getaddrinfo () failed with error : \n%d: %s\n", result, gai_strerror(WSAGetLastError()));
		WSACleanup();
		return 1;
	}

	// need to walk the linked list
	printf(" Addressing information :\n");

	int i = 0;
	for (temp = addr; temp != NULL; temp = temp->ai_next)
	{
		printf("\n Entry %d:\n", ++i);
		switch (temp->ai_family)
		{
		case AF_INET:
			printf("\t Address family : AF_INET \n");
			break;
		case AF_INET6:
			printf("\t Address family : AF_INET6 \n");
			break;
		}
		switch (temp->ai_protocol)
		{
		case IPPROTO_TCP:
			printf("\t Protocol : TCP \n");
			break;
		case IPPROTO_UDP:
			printf("\t Protocol : UDP \n");
			break;
		}
		switch (temp->ai_socktype)
		{
		case SOCK_STREAM:
			printf("\t Socket type : Stream \n");
			break;
		case SOCK_DGRAM:
			printf("\t Socket type : Datagram \n");
			break;
		}
	}
	return 0;
}


int NetworkServer::sendRecv(void)
{
	SOCKET s = NULL;
	// initialise socket library
	if (init())
	{
		printf(" Unable to initialise the Winsock library \n");
		exit(1);
	}

	// intiailise addressing information
	if (addressing() != 0)
	{
		printf(" Uanble to initialise addressing information \n");
		exit(1);
	}
	// create a socket for the server to listen on
	if ((s = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) == INVALID_SOCKET)
	{
		printf(" Unable to create a socket \n");
		printf(" Failed with error : %d\n%s\n", WSAGetLastError(), gai_strerror(WSAGetLastError()));
		exit(1);
	}
	else
	{
		printf("\n Socket created successfully. \n");
	}

	// bind to the socket created above
	if (bind(s, addr->ai_addr, addr->ai_addrlen) != 0)
	{
		printf(" Unable to bind to socket \n");
		printf(" Failed with error : %d\n%s\n", WSAGetLastError(), gai_strerror(WSAGetLastError()));
	}
	else
	{
		printf(" Bound to socket. \n");
	}
	// finished with addrinfo struct now

	freeaddrinfo(addr);
	// listen on the socket
	if (listen(s, BACKLOG) != 0)
	{
		printf(" Unable to listen on socket \n");
		printf(" Failed with error : %d\n%s\n", WSAGetLastError(), gai_strerror(WSAGetLastError()));
	}
	else
	{
		printf(" Listening on the socket. \n");
	}

	// continually accept new connections
	while (GameClass::GetGameClass().getCurrentState()!= GAME_EXIT)
	{
		cout << GameClass::GetGameClass().getCurrentState() << endl;
		printf("\n Waiting for connections ... \n");
		SOCKET inc = NULL;
		struct sockaddr_storage inc_addr;
		socklen_t inc_size = sizeof (inc_addr);
		// accept new connection from a client
		if ((inc = accept(s, (struct sockaddr *) & inc_addr, &inc_size)) == INVALID_SOCKET)
		{
			printf(" Unable to accept new connection \n");
			printf(" Failed with error : %d\n%s\n", WSAGetLastError(), gai_strerror(WSAGetLastError()));
		}
		else
		{
			printf(" Accepted a new connection ...\n");
		}

		// send message to the client
		char * hw = " Hello Client ";
		send(inc, hw, strlen(hw), 0);

		// receive message from client
		int bytesreceived;
		char buff[BUFFSIZE];
		int int_data;
		messageInfo info_data;
		Matrix4 mat_data;

		if ((bytesreceived = recv(inc, (char*)&mat_data, BUFFSIZE - 1, 0)) == -1)
		{
			printf(" Error receiving \n");
			printf(" Failed with error : %d\n%s\n", WSAGetLastError(), gai_strerror(WSAGetLastError()));
		}
		else
		{
			std::cout << " Server Message Recieved. " << std::endl;
			std::cout << " Recieved " << bytesreceived << " Bytes." << std::endl;
			std::cout << " Message is : " << mat_data << std::endl;
		}

		closesocket(inc);

	}
	closesocket(s);
	WSACleanup();
}