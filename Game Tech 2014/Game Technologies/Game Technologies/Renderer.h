/******************************************************************************
Class:Renderer		
Implements:OGLRendere
******************************************************************************/


#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/Frustum.h"
#include <algorithm>
#include "textmesh.h"
#include "PhysicsSystem.h"

class Renderer : public OGLRenderer	
{
public:
	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	void	SetCamera(Camera*c);
	void	AddNode(SceneNode* n);
	void	RemoveNode(SceneNode* n);

	//Statics
	static bool Initialise() 
	{
		instance = new Renderer(Window::GetWindow());
		return instance->HasInitialised();
	}

	static void Destroy() 
	{
		delete instance;
	}
	
	static Renderer&GetRenderer() { return *instance;}

protected:	
	Renderer(Window &parent);
	virtual ~Renderer(void);

	//Base Renderer Functionality
	void	BuildNodeLists(SceneNode* from);
	void	SortNodeLists();
	void	ClearNodeLists();
	void	DrawNodes();
	void	DrawNode(SceneNode*n);


	//Hud Display
	void	DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective);	//Provided From Tutorials
	void	displayInformation();
	void	toggleWireFrame();

	SceneNode*	root;
	Camera*		camera;
	Shader*		simpleShader;
	Shader*		textShader;				//Basic Shader for Drawing 2D text on screen (TexVertex, TexFragment)
	Font*		basicFont;			//Font for drawing text to screen


	Frustum		frameFrustum;
	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	static Renderer*	instance;


	int nodeCount = 0;
	int nbFrames = 0;
	float time = 0;						//Float variable to hold milliseconds, counts to 1000 useing msec to attain framerate
	int fps = 0;				//FPS string to use with DrawText
	bool wireFrame = false;
};

