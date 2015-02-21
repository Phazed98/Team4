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
#include "Frustum.h"
#include <algorithm>
//#include "textmesh.h"
#include "PhysicsSystem.h"

class Renderer : public GCMRenderer	
{
public:
	Renderer(void);
	~Renderer(void);
	
	void	SetCamera(Camera*c);
	void	AddNode(SceneNode* n);
	void	RemoveNode(SceneNode* n);

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

};
