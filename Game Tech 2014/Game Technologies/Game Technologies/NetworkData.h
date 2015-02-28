#pragma once

#include <string.h>
#include <vector>
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Vector3.h"
#include "../../nclgl/Quaternion.h"
#define MAX_PACKET_SIZE 1000000



enum PacketTypes
{
	INIT_CONNECTION = 0,
	ACTION_EVENT = 1,
};

struct messageInfo
{
	int objectID;
	Vector3 Position;
	Quaternion Orientation;
	OBJType objectType;
};


struct Packet
{
	unsigned int packet_type;
	int packet_integer;
	messageInfo data[200];

	void serialize(char* data)
	{
		memcpy(data, this, sizeof(Packet));
	}

	void deserialize(char* data)
	{
		memcpy(this, data, sizeof(Packet));
	}
};