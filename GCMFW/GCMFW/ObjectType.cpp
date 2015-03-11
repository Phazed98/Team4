#include "ObjectType.h"
#include "PhysicsSystem.h"
#include "MathHelper.h"


ObjectType::ObjectType(SceneNode* s, PhysicsNode* p, int _type, int _subType) : GameEntity(s, p)
{
	type = _type;
	subType = _subType;
	state = 0;
	SetInitialAttributes();
	length = 800.0f;
	random = 1;
}


ObjectType::~ObjectType()
{

}

void ObjectType::Update(float msec)
{
	if (type == 0)
	{
		float speed = PhysicsSystem::GetTrackSpeed();
		//float speed = 10.0f;
		physicsNode->SetPosition(physicsNode->GetPosition() + Vector3(0, 0, speed));

		if (state == 0)
		{
			if (abs(physicsNode->GetPosition().getZ() - -5550) > length* random)
			{
				state = 1;
			}
		}
		else if (state == 2)
		{
			if (physicsNode->GetPosition().getZ() > (5000))
			{
				state = 3;
			}
		}

	}
	else if (type == 2)
	{
		float speed = PhysicsSystem::GetTrackSpeed();
		physicsNode->SetPosition(physicsNode->GetPosition() + Vector3(0, 0, speed));
		checkPointTimer -= msec;
		//PhysicsSystem::GetPhysicsSystem().SetCheckPointTimer(checkPointTimer / 1000);

		if (physicsNode->GetPosition().getZ() > 0 && increaseTimer)
		{
			checkPointTimer += 10000 + resetDistance;

			increaseTimer = false;
		}

		if (physicsNode->GetPosition().getZ() > (resetDistance))
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
		physicsNode->SetPosition(Vector3(0, TILE_CENTRE_OFFSET, -5550));
		renderNode->SetColour(Vector4(1, 0, 0, 1));
	}
	else if (subType == 1) // Right
	{
		physicsNode->SetPosition(Vector3(TILE_CENTRE_OFFSET, 0, -5550));
		Vector4 rotVec = MathHelper::vec3ToEuler(0, 0, 90);
		physicsNode->SetOrientation(Quat(rotVec.getX(), rotVec.getY(), rotVec.getZ(), rotVec.getW()));
		renderNode->SetColour(Vector4(0, 0, 1, 1));
	}
	else if (subType == 2) // Bottom
	{
		physicsNode->SetPosition(Vector3(0, -TILE_CENTRE_OFFSET, -5550));
		renderNode->SetColour(Vector4(0.1f, 0.1f, 0.1f, 1.0f));
	}
	else if (subType == 3) // Left
	{
		physicsNode->SetPosition(Vector3(-TILE_CENTRE_OFFSET, 0, -5550));
		Vector4 rotVec = MathHelper::vec3ToEuler(0, 0, 90);
		physicsNode->SetOrientation(Quat(rotVec.getX(), rotVec.getY(), rotVec.getZ(), rotVec.getW()));
		renderNode->SetColour(Vector4(0, 1, 0, 1));
	}
}

void ObjectType::reset()
{
	state = 0;
	SetInitialAttributes();
	length = 800.0f;
	random = 1;

	//create obstacle
}

void ObjectType::SetPos(Vector3 pos)
{
	physicsNode->SetPosition(pos);

}