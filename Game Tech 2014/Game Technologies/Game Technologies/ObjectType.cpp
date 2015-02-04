#include "ObjectType.h"


ObjectType::ObjectType(SceneNode* s, PhysicsNode* p, int _type, int _subType) : GameEntity(s, p)
{
	type = _type;
	subType = _subType;
	SetInitialPosition();
}


ObjectType::~ObjectType()
{

}

void ObjectType::Update(float msec)
{

}

void ObjectType::SetInitialPosition()
{
	if (subType == 1) // Top
	{
		physicsNode->SetPosition(Vector3(0, 500, 0));
	}
	else if (subType == 2) // Right
	{
		physicsNode->SetPosition(Vector3(500, 0, 0));
	}
	else if (subType == 3) // Down
	{
		physicsNode->SetPosition(Vector3(0, -500, 0));
		physicsNode->SetOrientation(Quaternion(0, 0, 1, 1));
	}
	else if (subType == 4) // Left
	{
		physicsNode->SetPosition(Vector3(-500, 0, 0));
		physicsNode->SetOrientation(Quaternion(0, 0, 1, 1));
	}
}