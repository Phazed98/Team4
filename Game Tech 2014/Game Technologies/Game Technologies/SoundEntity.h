#pragma once
#include "../../nclgl/SceneNode.h"
#include "PhysicsNode.h"
#include "../../nclgl/SoundEmitter.h"
#include "GameEntity.h"
//#include "../../nclgl/SoundSystem.h"

class SoundEntity :public GameEntity{
public:
	SoundEntity(void);
	SoundEntity(SoundEmitter* snd, SceneNode* s, PhysicsNode* p);
	virtual ~SoundEntity(void);

	SceneNode&		GetRenderNode()		{ return *renderNode;}
	PhysicsNode&	GetPhysicsNode()	{ return *physicsNode;}
	SoundEmitter&   GetSoundEmitter()   { return *soundEmitter;}

	void			ConnectToSystems();
	void			DisconnectFromSystems();

protected:
	SceneNode*		renderNode;
	PhysicsNode*	physicsNode;
	SoundEmitter*   soundEmitter;
};
