#include "NetworkClient.h"

int NetworkClient::init()
{
	// call to startup
	int error = WSAStartup(MAKEWORD(HIGHVERSION, LOWVERSION), &wsaClientData);
	if (error != 0)
	{
		fprintf(stderr, " WSAStartup failed with error : %d.\n", error);
		return 1;
	}
	// check version values

	if (LOBYTE(wsaClientData.wVersion) != LOWVERSION || HIBYTE(wsaClientData.wVersion) != HIGHVERSION)
	{
		printf(" The version requested cannot be supported .\n");
		printf(" Version set is %d.%d\n", LOBYTE(wsaClientData.wVersion), HIBYTE(wsaClientData.wVersion));
		WSACleanup();
		return 1;
	}
	else
	{
		printf(" The Winsock API has been successfully initialised .\n"
			" You are using version %d.%d.\n\n", HIBYTE(wsaClientData.wVersion), LOBYTE(wsaClientData.wVersion));
		return 0;
	}
}

int NetworkClient::addressing()
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

	result = getaddrinfo(HOST, PORT, &hints, &clientaddr);

	if (result != 0)
	{
		printf(" getaddrinfo () failed with error : \n%d: %s\n", result, gai_strerror(WSAGetLastError()));
		WSACleanup();
		return 1;
	}

	// need to walk the linked list
	printf(" Addressing information :\n");

	int i = 0;
	for (temp = clientaddr; temp != NULL; temp = temp->ai_next)
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

int NetworkClient::sendRecv(void)
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
	if ((s = socket(clientaddr->ai_family, clientaddr->ai_socktype, clientaddr->ai_protocol)) == INVALID_SOCKET)
	{
		printf(" Unable to create a socket \n");
		printf(" Failed with error : %d\n%s\n", WSAGetLastError(), gai_strerror(WSAGetLastError()));
		exit(1);
	}
	else
	{
		printf("\n Socket created successfully .\n");
	}

	// connect to the server
	if (connect(s, clientaddr->ai_addr, clientaddr->ai_addrlen) != 0)
	{
		printf(" Unable to connect to server \n");
		printf(" Failed with error : %d\n%s\n", WSAGetLastError(), gai_strerror(WSAGetLastError()));
	}
	else
	{
		printf("\n Connected to the server .\n");
	}

	// finished with addrinfo struct now
	freeaddrinfo(clientaddr);

	// accept message from server and close
	int bytesreceived;
	char buff[BUFFSIZE];

	if ((bytesreceived = recv(s, buff, BUFFSIZE - 1, 0)) == -1)
	{
		printf(" Error receiving \n");
		printf(" Failed with error : %d\n%s\n", WSAGetLastError(), gai_strerror(WSAGetLastError()));
	}
	else
	{
		buff[bytesreceived] = '\0';
		printf("\n\n Client Message received . Received %d bytes. \n Message is: %s", bytesreceived, buff);
		printf("\n\n");
	}


	int int_data = 191123;
	messageInfo info_data;
	Matrix4 mat_data;
	mat_data.ToIdentity();
	info_data.a = 5;
	info_data.b = 3.08f;

	send(s, (char*)&mat_data, sizeof(messageInfo), 0);

	closesocket(s);
	WSACleanup();
}