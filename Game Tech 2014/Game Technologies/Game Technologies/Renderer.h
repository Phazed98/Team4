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
#include "Button.h"
#include "SpaceshipSceneNode.h"
#include "FireParticleSystem.h"


class ChaseCamera;

class Renderer : public OGLRenderer	
{
public:
	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	void	SetCamera(ChaseCamera*c);
	void	AddNode(SceneNode* n);
	void	RemoveNode(SceneNode* n);
	void	RenderMenu();
	void	RenderLoading(int percent, string Message);
	ChaseCamera* GetCamera(){ return camera; }
	Matrix4 GetProjMatrix(){ return projMatrix; }
	void SetSpaceshipSceneNode(SpaceshipSceneNode* ssn){ spaceship_scene_node = ssn; }
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
	ChaseCamera*	camera;
	Shader*		simpleShader;
	Shader*		textShader;				//Basic Shader for Drawing 2D text on screen (TexVertex, TexFragment)
	Shader*		texturedShader;
	Shader*		menuShader;
	Font*		basicFont;			//Font for drawing text to screen
	Mesh*		quad;

	Button*	playButton;
	Button*	resetButton;
	Button*	exitButton;



	Frustum		frameFrustum;
	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	static Renderer*	instance;


	int nodeCount = 0;
	int nbFrames = 0;
	float time = 0;						//Float variable to hold milliseconds, counts to 1000 useing msec to attain framerate
	int fps = 0;				//FPS string to use with DrawText
	bool wireFrame = false;

	//for motion blur
	GLuint motion_blur_FBO;
	GLuint motion_blur_ColourTex;
	GLuint motion_blur_DepthTex;
	bool CreatMotionBlurBuffer();
	void PresentMotionBlur();
	Mesh* quad_motion_blur;
	Shader* motion_blur_shader;
	Matrix4 previous_viewMatrix;
	Matrix4 current_viewMatrix;
	void RenderMotionBlur();
	void RenderWithoutPostProcessing();
	bool render_motion_blur;

	SpaceshipSceneNode* spaceship_scene_node;
	void DrawAfterBurner();
	float msec;
};

