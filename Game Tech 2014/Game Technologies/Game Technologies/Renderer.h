/******************************************************************************
Class:Renderer		
Implements:OGLRendere
******************************************************************************/


#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/Frustum.h"
#include "../../nclgl/OBJMesh.h"
#include <algorithm>
#include "textmesh.h"
#include "PhysicsSystem.h"
#include "Button.h"
#include "SpaceshipSceneNode.h"
#include "FireParticleSystem.h"
#include "EarthParticleSystem.h"
#include "TornadoSceneNode.h"
#include "FireSceneNode.h"
#include "GeyserSceneNode.h"
#include "LifeformSceneNode.h"
#include "PointLight.h"

#define PAUSE_BUTTONS_SIZE 2
#define	MAIN_BUTTONS_SIZE 6

enum buttonPressed
{
	SOLO,
	HOST_2,
	HOST_3,
	HOST_4,
	CLIENT,
	EXIT,
	NO_PRESSED,
};

class ChaseCamera;

class Renderer : public OGLRenderer	
{
public:
	virtual void RenderScene();
	virtual void UpdateScene(float msec);
	void fullyInit();

	void	SetCamera(ChaseCamera*c);
	void	AddNode(SceneNode* n);
	void	RemoveNode(SceneNode* n);
	void	RenderPauseMenu();
	void	RenderUI();
	buttonPressed	RenderMainMenu();
	void	RenderLoading(int percent, string Message);
	ChaseCamera* GetCamera(){ return camera; }
	Matrix4 GetProjMatrix(){ return projMatrix; }
	void SetSpaceshipSceneNode(SpaceshipSceneNode* ssn){ spaceship_scene_node = ssn; }

	void SetTornadoSceneNode(TornadoSceneNode* node){ tornadoNode.push_back(node); }
	void SetGeyserSceneNode(GeyserSceneNode* node){ geyserNode.push_back(node); }
	void SetYellowformSceneNode(SceneNode* node){ yellowNode.push_back(node); }
	void SetRedformSceneNode(SceneNode* node){ redNode.push_back(node); }
	void SetBlueformSceneNode(SceneNode* node){ blueNode.push_back(node); }

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

	void SetFireSceneNode(FireSceneNode* node){ fireNode.push_back(node); }
	string getIPAddress() { return ipAddress; }

	vector<PointLight *> point_lights;

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

	//steven added for motion blur
	GLuint motion_blur_FBO;
	GLuint motion_blur_ColourTex;
	bool CreatMotionBlurBuffer();
	void PresentMotionBlur();
	Mesh* quad_motion_blur;
	Shader* motion_blur_shader;
	Matrix4 previous_viewMatrix;
	Matrix4 current_viewMatrix;

	SpaceshipSceneNode* spaceship_scene_node;
	void DrawAfterBurner();
	void DrawShield();
	float msec;

	//steven added for render particles
	GLuint particle_FBO;
	GLuint particle_ColourTex;
	bool CreatParticleBuffer();
	Mesh* quad_particle;
	void RenderParticleToTexture();

	//steven added for background
	void RenderBackground();
	Matrix4 modeltemp;
	Matrix4 viewtemp;
	Matrix4 projtemp;
	Matrix4 texturetemp;
	void PushMatrix();
	void PopMatrix();
	void MatrixToIdentity();
	GLuint background[4];
	Shader*	backgroundShader;

	GLuint background_ColourTex;


	//Menu Buttons
	Mesh*		mainMenuQuad;
	Mesh*		ipQuad;
	Button*		pauseMenuButtons[PAUSE_BUTTONS_SIZE];
	Button*		mainMenuButtons[MAIN_BUTTONS_SIZE];
	int			mainButtonIndex;
	int			pauseButtonIndex;
	string		ipAddress;

	//UI Elements
	Mesh* uiQuad;
	Mesh* blueBar;
	Mesh* yellowBar;
	Mesh* redBar;
	Mesh* greenBar;
	Mesh* buttonB;
	Mesh* buttonX;
	Mesh* buttonY;
	float planeSwitchCD;
	float slowPowerUpDuration;
	float immunePowerUpDuration;
	float cdPowerUpDuration;

	vector<TornadoSceneNode*> tornadoNode;
	EarthParticleSystem tornado_system;
	void DrawTornado();

	vector<GeyserSceneNode*> geyserNode;
	EarthParticleSystem geyser_system;
	void DrawGeyser();

	vector<SceneNode*> yellowNode;
	vector<SceneNode*> blueNode;
	vector<SceneNode*> redNode;
	EarthParticleSystem yellow_system;
//	EarthParticleSystem red_system;
//	EarthParticleSystem blue_system;
	void Draw_powerup();
//	void Draw_red_powerup();
//	void Draw_blue_powerup();

	//steven added for rendering different objects
	Shader* water_plane_shader;
	float total_sec_pass;

	EarthParticleSystem galaxy_system;
	EarthParticleSystem galaxyShield;

	EarthParticleSystem awesomeness;

	void DrawFire();

	vector<FireSceneNode*> fireNode;
	FireParticleSystem fire_system;

	bool fullyInitialised;

	//steven added for defer rendering
	bool InitDSBuffer();
	GLuint defer_FBO;
	GLuint defer_textures[4];
	GLuint defer_depth_texture;
	Shader* defer_shader;
	void DSFinalPass();
	void DSGeometryPass();
	void deferRenderPass();
	
	OBJMesh* point_light_sphere;
	Shader* point_light_shader;
	void DSPointLightsPass(PointLight* pt);
	void DSStencilPass(PointLight* pt);
	GLuint defer_final_texture;
	void DSDirectionalLightPass();
	Mesh* screen_quad;
	Shader* dir_light_shader;

//for the background changes
	GLuint pre_back;
	GLuint cur_back;

	void FinalPostProcessing();
	Shader* post_processing_shader;

	GameTimer postPtimer;
	float* motionblurTimer;
	float* deferTimer;
	float* postprocessTimer;

	bool explosion;
	float explosion_time;

	
};

