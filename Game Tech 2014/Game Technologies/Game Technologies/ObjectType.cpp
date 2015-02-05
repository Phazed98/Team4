#include "ObjectType.h"


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
		physicsNode->SetPosition(physicsNode->GetPosition() + Vector3(0, 0, -100.5f));

		if (state == 0)
		{
			
			if (physicsNode->GetPosition().z < -(length)* random)
			{
				state = 1;
			}
		}
		else if (state == 2)
		{
			if (physicsNode->GetPosition().z < -(length*15))
			{
				state = 3;
			}
		}
	}
}

void ObjectType::SetInitialAttributes()
{
	
	if (subType == 0) // Top
	{
		physicsNode->SetPosition(Vector3(0, 500, 0));
		renderNode->SetColour(Vector4(1, 0, 0, 1));
	}
	else if (subType == 1) // Right
	{
		physicsNode->SetPosition(Vector3(500, 0, 0));
		physicsNode->SetOrientation(Quaternion(0, 0, 1, 1));
		renderNode->SetColour(Vector4(0, 0, 1, 1));
	}
	else if (subType == 2) // Bottom
	{
		physicsNode->SetPosition(Vector3(0, -500, 0));
		renderNode->SetColour(Vector4(1, 1, 1, 1));
	}
	else if (subType == 3) // Left
	{
		physicsNode->SetPosition(Vector3(-500, 0, 0));
		physicsNode->SetOrientation(Quaternion(0, 0, 1, 1));
		renderNode->SetColour(Vector4(0, 1, 0, 1));
	}
}

void ObjectType::reset()
{
	state = 0;
	SetInitialAttributes();
	length = 800.0f;
	random = 1;
}