#include "Renderer.h"
#include "PhysicsSystem.h"
#include "../../nclgl/SoundSystem.h"
#include "SoundEntity.h"

SoundEntity::SoundEntity(void)	{
	renderNode	= NULL;
	physicsNode = NULL;
	soundEmitter = NULL;
}

SoundEntity::SoundEntity(SoundEmitter* snd, SceneNode* s, PhysicsNode* p) {
	soundEmitter = snd;
	renderNode	= s;
	physicsNode = p;
}

SoundEntity::~SoundEntity(void)	{
	DisconnectFromSystems();

	delete soundEmitter;
	delete renderNode;
	delete physicsNode;
}



void	SoundEntity::ConnectToSystems() {
	if(renderNode) {
		Renderer::GetRenderer().AddNode(renderNode);
	}

	if(physicsNode) {
		PhysicsSystem::GetPhysicsSystem().AddNode(physicsNode);
	}

	if(soundEmitter) {
		SoundSystem::GetSoundSystem()->AddSoundEmitter(soundEmitter);
	}

	if(renderNode  && soundEmitter) {
		soundEmitter->SetTarget(renderNode);
	}
}

void	SoundEntity::DisconnectFromSystems() {
	if(renderNode) {
		Renderer::GetRenderer().RemoveNode(renderNode);
	}

	if(physicsNode) {
		PhysicsSystem::GetPhysicsSystem().RemoveNode(physicsNode);
	}

	if(soundEmitter) {
		SoundSystem::GetSoundSystem()->RemoveSoundEmitter(soundEmitter);
	}
}