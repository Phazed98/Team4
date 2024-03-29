#pragma once;

# include <stdio.h>
# include <winsock2.h>
# include <ws2tcpip.h>
#include <string>
#include <iostream>
#include "../../nclgl/Matrix4.h"
#include "../../nclgl/Vector3.h"

#include "MyGame.h"

# define HIGHVERSION 2
# define LOWVERSION 2
# define HOST "127.0.0.1"
# define PORT "4376"
# define FAMILY AF_UNSPEC
# define TYPE SOCK_STREAM
# define FLAGS AI_PASSIVE
# define PROTOCOL IPPROTO_TCP
# define BACKLOG 10
# define BUFFSIZE 2048

#pragma comment(lib, "Ws2_32.lib")


struct messageInfo
{
	int objectID;
	Vector3 Position;
	Quaternion Orientation;
};

