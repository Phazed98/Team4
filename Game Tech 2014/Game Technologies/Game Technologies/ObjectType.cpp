#include "ObjectType.h"
#include "PhysicsSystem.h"


ObjectType::ObjectType(SceneNode* s, PhysicsNode* p, int _type, int _subType) : GameEntity(s, p)
{
	type = _type;
	subType = _subType;
	state = 0;
	SetInitialAttributes();
	length = 800.0f;
	random = 1;
	resetDistance = END_POSITION;
	checkPointTimer = 20000;
	increaseTimer = true;
}


ObjectType::~ObjectType()
{

}

void ObjectType::Update(float msec)
{
	if (type == 0)
	{
		float speed = PhysicsSystem::GetTrackSpeed();
		physicsNode->SetPosition(physicsNode->GetPosition() + Vector3(0, 0, speed));

		if (state == 0)
		{

			if (abs(physicsNode->GetPosition().z - INITIAL_Z_POSITION) > length* random && physicsNode->GetPosition().z > INITIAL_Z_POSITION)
			{
				state = 1;
			}
		}
		else if (state == 2)
		{
			if (physicsNode->GetPosition().z > (END_POSITION))
			{
				state = 3;
			}
		}
	}

	if (type == 2)
	{
		float speed = PhysicsSystem::GetTrackSpeed();
		physicsNode->SetPosition(physicsNode->GetPosition() + Vector3(0, 0, speed));
		checkPointTimer -= msec;
		PhysicsSystem::GetPhysicsSystem().SetCheckPointTimer(checkPointTimer / 1000);

		if (physicsNode->GetPosition().z > 0 && increaseTimer)
		{
			checkPointTimer += 10000 + resetDistance;

			increaseTimer = false;
		}

		if (physicsNode->GetPosition().z > (resetDistance))
		{
			reset();
			resetDistance += 1000;
			increaseTimer = true;
		}

	}

}


void ObjectType::SetInitialAttributes()
{

	if (subType == 0) // Top
	{
		physicsNode->SetPosition(Vector3(0, TILE_CENTRE_OFFSET, INITIAL_Z_POSITION));
		renderNode->SetColour(Vector4(1, 0, 0, 1));
	}
	else if (subType == 1) // Right
	{
		physicsNode->SetPosition(Vector3(TILE_CENTRE_OFFSET, 0, INITIAL_Z_POSITION));
		physicsNode->SetOrientation(Quaternion(0, 0, 1, 1));
		renderNode->SetColour(Vector4(0, 0, 1, 1));
	}
	else if (subType == 2) // Bottom
	{
		physicsNode->SetPosition(Vector3(0, -TILE_CENTRE_OFFSET, INITIAL_Z_POSITION));
		renderNode->SetColour(Vector4(0.1f, 0.1f, 0.1f, 1.0f));
	}
	else if (subType == 3) // Left
	{
		physicsNode->SetPosition(Vector3(-TILE_CENTRE_OFFSET, 0, INITIAL_Z_POSITION));
		physicsNode->SetOrientation(Quaternion(0, 0, 1, 1));
		renderNode->SetColour(Vector4(0, 1, 0, 1));
	}
	else if (subType == 4) // Middle
	{
		physicsNode->SetPosition(Vector3(0, 0, INITIAL_Z_POSITION));
	}
}

void ObjectType::reset()
{
	state = 0;
	SetInitialAttributes();
	length = 800.0f;
	random = 1;
}

void ObjectType::SetPos(Vector3 pos)
{
	physicsNode->SetPosition(pos);
}