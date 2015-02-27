#pragma once

#include "../../nclgl/SceneNode.h"
#include "PhysicsNode.h"

enum EntityType
{
	ENTITY_DEFAULT,
	ENTITY_PLANE,
	ENTITY_OBJECT,
	ENTITY_OBSTACLE,
	ENTITY_PLAYER,
	ENTITY_VEHICLE,
};

class GameEntity	
{
public:

	static int globalID;

	friend class GameClass;

	GameEntity(void);
	//Sam - new constructor added for vehicle - not having the physics node connect to systems!
	GameEntity(SceneNode* s);
	GameEntity(SceneNode* s, PhysicsNode* p);
	virtual ~GameEntity(void);

	virtual void	Update(float msec);

	SceneNode&		GetRenderNode()		{ return *renderNode;}
	PhysicsNode&	GetPhysicsNode()	{ return *physicsNode;}

	void			ConnectToSystems();
	void			DisconnectFromSystems();
	int objectID;

protected:
	SceneNode*		renderNode = NULL;
	PhysicsNode*	physicsNode = NULL;
	
};

