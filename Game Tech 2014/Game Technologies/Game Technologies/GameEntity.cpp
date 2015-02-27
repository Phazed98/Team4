#include "GameEntity.h"
#include "Renderer.h"
#include "PhysicsSystem.h"

int GameEntity::globalID = 0;

GameEntity::GameEntity(void)	{
	renderNode	= NULL;
	physicsNode = NULL;
	objectID = globalID++;
}

//Added by Sam for creating vehicles
GameEntity::GameEntity(SceneNode* s) : renderNode(s)
{
	objectID = globalID++;
}

GameEntity::GameEntity(SceneNode* s, PhysicsNode* p) :
renderNode(s), physicsNode(p)
{
	objectID = globalID++;
}

GameEntity::~GameEntity(void)	{
	DisconnectFromSystems();

	delete renderNode;
	delete physicsNode;
}

void	GameEntity::Update(float msec) {

}

//NOTE - CALL LAST DURING OBJECT CREATION
void	GameEntity::ConnectToSystems() {
	if (renderNode) {
		Renderer::GetRenderer().AddNode(renderNode);
	}

	if (physicsNode) {
		PhysicsSystem::GetPhysicsSystem().AddNode(physicsNode);
	}

	if (renderNode && physicsNode) {
		physicsNode->SetTarget(renderNode);

		//Added by Sam, sets the AABBHalfLength, best place for it really
		physicsNode->ConfigureAABBHalfLength();
	}
}

void	GameEntity::DisconnectFromSystems() {
	if(renderNode) {
		Renderer::GetRenderer().RemoveNode(renderNode);
	}

	if(physicsNode) {
		PhysicsSystem::GetPhysicsSystem().RemoveNode(physicsNode);
	}
}