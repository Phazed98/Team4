/******************************************************************************
Class:Renderer
Implements:
Author:Rich Davison
Description: For consistency with the Graphics for Games module, we're deriving
a 'renderer' from the actual API specific parent class. This is the one you
should be fiddling with! Add whatever you like, here.

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*//////////////////////////////////////////////////////////////////////////////

#pragma once
#include "GCMRenderer.h"
#include "Mesh.h"
#include "SceneNode.h"
#include "Camera.h"
#include "ChaseCamera.h"
#include "Frustum.h"
#include <algorithm>
//#include "textmesh.h"
#include "PhysicsSystem.h"
#include "Button.h"

#define PAUSE_BUTTONS_SIZE 2
#define	MAIN_BUTTONS_SIZE 3

enum buttonPressed
{
	SOLO,
	SPLIT,
	EXIT,
	NO_PRESSED,
};

class Renderer : public GCMRenderer	
{
public:
	Renderer(void);
	~Renderer(void);
	
	void	SetCamera(ChaseCamera *c); 
	//void	SetCamera(Camera*c);
	void	AddNode(SceneNode* n);
	void	RemoveNode(SceneNode* n);
	void	RenderPauseMenu();
	buttonPressed	RenderMainMenu();
	void	RenderLoading(int percent);
	void SetSpaceshipSceneNode(SpaceshipSceneNode* ssn){ spaceship_scene_node = ssn; }


	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	//Statics
	static bool Initialise()
	{
		instance = new Renderer();
		return instance->HasInitialised();
	}

	static void Destroy()
	{
		delete instance;
	}

	static Renderer&GetRenderer() { return *instance; }

protected:
	//For Later
	//Renderer(Window &parent);
	//virtual ~Renderer(void);

	//Base Renderer Functionality
	void	BuildNodeLists(SceneNode* from);
	void	SortNodeLists();
	void	ClearNodeLists();
	void	DrawNodes();
//	void	DrawNode(SceneNode*n);

	Frustum		frameFrustum;
	std::vector<SceneNode*> transparentNodeList;
	std::vector<SceneNode*> nodeList;

	static Renderer*	instance;

	int nodeCount;
	int nbFrames;
	float time;					//Float variable to hold milliseconds, counts to 1000 useing msec to attain framerate
	int fps;				//FPS string to use with DrawText
	bool wireFrame;

	//Menu Buttons
	SceneNode* mainMenuSceneNode;
	SceneNode* selectorSceneNode;
	Mesh*		selectorMesh;
	Mesh*		mainMenuQuad;
	Mesh*		BackGrounMesh;
	SceneNode*	BackGroundSceneNode;
	SceneNode* pauseMenuButtons[PAUSE_BUTTONS_SIZE];
	SceneNode* mainMenuButtons[MAIN_BUTTONS_SIZE];
	int mainButtonIndex;
	int pauseButtonIndex;
	int coolDown;

	SpaceshipSceneNode* spaceship_scene_node;

};
