#pragma once

#include <string.h>
#include <vector>
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Vector3.h"
#include "../../nclgl/Quaternion.h"
#define MAX_PACKET_SIZE 1000000
#define DATA_SIZE 2048



enum PacketTypes
{
	INIT_CONNECTION = 0,
	ACTION_EVENT = 1,
	START_GAME = 2,
	CLIENT_POSITION_DATA = 3,
	PLAYERS_DATA = 4,
	GAME_PAUSE,
	GAME_UNPAUSE,
};

struct messageInfo
{
	int objectID;
	float offset;
	Vector3 Position;
	Quaternion Orientation;
};


struct Packet
{
	unsigned int packet_type;
	int packet_integer;
	char data[DATA_SIZE];

	void serialize(char* data)
	{
		memcpy(data, this, sizeof(Packet));
	}

	void deserialize(char* data)
	{
		memcpy(this, data, sizeof(Packet));
	}
};