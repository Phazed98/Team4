#include "Renderer.h"
#include "ChaseCamera.h"
#include "GamePadController.h"
#include "../../nclgl/OBJMesh.h"

Renderer* Renderer::instance = NULL;

Renderer::Renderer(Window &parent) : OGLRenderer(parent)
{
	tornado_system.InitParticleSystem(0, Vector3(0, 10, 0));
	galaxy_system.InitParticleSystem(1, Vector3(0, 0, -200));
	fire_system.InitParticleSystem(0, Vector3(0, 0, 0));
	
	menuShader = new Shader(SHADERDIR"menuVertex.glsl", SHADERDIR"menuFragment.glsl");
	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);
	textShader = new Shader(SHADERDIR"TexVertex.glsl", SHADERDIR"TexFragment.glsl");
	ipAddress = "127.0.0.1";

	if (!menuShader->LinkProgram() || !textShader->LinkProgram())
	{
		cout << "error in link shaders" << endl;
		return;
	}

	pauseButtonIndex = 0;
	mainButtonIndex = 0;

	mainMenuQuad = Mesh::GenerateQuad();
	mainMenuQuad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Loading Screen.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	ipQuad = Mesh::GenerateQuad();
	ipQuad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"IPBox.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));


	//Main Menu Buttons

	//Solo
	mainMenuButtons[0] = new Button(Mesh::GenerateQuad());
	mainMenuButtons[0]->SetDefaultTexture(SOIL_load_OGL_texture(TEXTUREDIR"MenuButtons/SoloButton.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	mainMenuButtons[0]->SetHighlightTexture(SOIL_load_OGL_texture(TEXTUREDIR"MenuButtons/SoloButtonHighlighted.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	//Host2
	mainMenuButtons[1] = new Button(Mesh::GenerateQuad());
	mainMenuButtons[1]->SetDefaultTexture(SOIL_load_OGL_texture(TEXTUREDIR"MenuButtons/Host2Button.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	mainMenuButtons[1]->SetHighlightTexture(SOIL_load_OGL_texture(TEXTUREDIR"MenuButtons/Host2ButtonHL.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	//Host3
	mainMenuButtons[2] = new Button(Mesh::GenerateQuad());
	mainMenuButtons[2]->SetDefaultTexture(SOIL_load_OGL_texture(TEXTUREDIR"MenuButtons/Host3Button.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	mainMenuButtons[2]->SetHighlightTexture(SOIL_load_OGL_texture(TEXTUREDIR"MenuButtons/Host3ButtonHL.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	//Host4
	mainMenuButtons[3] = new Button(Mesh::GenerateQuad());
	mainMenuButtons[3]->SetDefaultTexture(SOIL_load_OGL_texture(TEXTUREDIR"MenuButtons/Host4Button.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	mainMenuButtons[3]->SetHighlightTexture(SOIL_load_OGL_texture(TEXTUREDIR"MenuButtons/Host4ButtonHL.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	//Client
	mainMenuButtons[4] = new Button(Mesh::GenerateQuad());
	mainMenuButtons[4]->SetDefaultTexture(SOIL_load_OGL_texture(TEXTUREDIR"MenuButtons/clientButton.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	mainMenuButtons[4]->SetHighlightTexture(SOIL_load_OGL_texture(TEXTUREDIR"MenuButtons/clientButtonHL.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	//Exit
	mainMenuButtons[5] = new Button(Mesh::GenerateQuad());
	mainMenuButtons[5]->SetDefaultTexture(SOIL_load_OGL_texture(TEXTUREDIR"MenuButtons/MainExitButton.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	mainMenuButtons[5]->SetHighlightTexture(SOIL_load_OGL_texture(TEXTUREDIR"MenuButtons/MainExitButtonHL.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	//Pause Menu Buttons

	//Resume
	pauseMenuButtons[0] = new Button(Mesh::GenerateQuad());
	pauseMenuButtons[0]->SetDefaultTexture(SOIL_load_OGL_texture(TEXTUREDIR"MenuButtons/resume.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	pauseMenuButtons[0]->SetHighlightTexture(SOIL_load_OGL_texture(TEXTUREDIR"MenuButtons/resumeHighlighted.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	//Exit
	pauseMenuButtons[1] = new Button(Mesh::GenerateQuad());
	pauseMenuButtons[1]->SetDefaultTexture(SOIL_load_OGL_texture(TEXTUREDIR"MenuButtons/ExitButton.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	pauseMenuButtons[1]->SetHighlightTexture(SOIL_load_OGL_texture(TEXTUREDIR"MenuButtons/ExitButtonHighlighted.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	instance = this;

	init = true;
}

Renderer::~Renderer(void)
{
	delete root;
	delete simpleShader;
	delete textShader;
	delete motion_blur_shader;
	delete quad;
	delete quad_motion_blur;

	delete camera;

	glDeleteFramebuffers(1, &motion_blur_FBO);
	glDeleteTextures(1, &motion_blur_ColourTex);
	glDeleteTextures(1, &motion_blur_DepthTex);

	currentShader = NULL;
}

void Renderer::fullyInit()
{
	camera = NULL;
	total_sec_pass = 0;
	cd = 10000;
	root = new SceneNode();
	
	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	background[0] = SOIL_load_OGL_texture(TEXTUREDIR"background2.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	glBindTexture(GL_TEXTURE_2D, background[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);


	water_plane_shader = new Shader(SHADERDIR"plane-shader/displace3DVert.glsl", SHADERDIR"plane-shader/TexturedFragment.glsl");

	//add by steven for motion blur
	quad_motion_blur = Mesh::GenerateQuad();
	render_motion_blur = false;

	motion_blur_shader = new Shader(SHADERDIR"MotionBlurVertex.glsl", SHADERDIR"MotionBlurFragment.glsl");

	simpleShader = new Shader(SHADERDIR"TechVertex.glsl", SHADERDIR"TechFragment.glsl");
	textShader = new Shader(SHADERDIR"TexVertex.glsl", SHADERDIR"TexFragment.glsl");
	backgroundShader = new Shader(SHADERDIR"BackgroundVertex.glsl", SHADERDIR"TexturedFragment.glsl");


	quad = Mesh::GenerateQuad();
	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Loading Screen.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	quad->SetColour(new Vector4(1, 0, 0, 1));

	
	//--------------------------------------------------GUI--------------------------------------------------------------------------------
	uiQuad = Mesh::GenerateQuad();
	uiQuad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"/GUITextures/score.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	cooldownBar = Mesh::GenerateQuad();
	cooldownBar->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"/GUITextures/cd.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	buttonB = Mesh::GenerateQuad();
	buttonB->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"/GUITextures/bigB.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	buttonX = Mesh::GenerateQuad();
	buttonX->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"/GUITextures/bigX.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	buttonY = Mesh::GenerateQuad();
	buttonY->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"/GUITextures/bigY.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	//--------------------------------------------------GUI--------------------------------------------------------------------------------

	if (!quad->GetTexture()){
		cout << "error in loading screen texture!!" << endl;
		return;
	}

	if (!simpleShader->LinkProgram() || !textShader->LinkProgram() ||
		!backgroundShader->LinkProgram() || !menuShader->LinkProgram() ||
		!motion_blur_shader->LinkProgram() || !water_plane_shader->LinkProgram())
	{
		cout << "error in link shaders" << endl;
		return;
	}

	if (!CreatMotionBlurBuffer()){
		//initilize the motion blur buffer
		//one depth buffer and one color buffer
		//one frame buffer 
		cout << "error in init motion blur buffer!!" << endl;
		return;
	}
	if (!CreatParticleBuffer()){
		//initilize the motion blur buffer
		//one depth buffer and one color buffer
		//one frame buffer 
		cout << "error in init particle buffer!!" << endl;
		return;
	}

	//Sam - moving here for scoping reasons
	OBJMesh* PlayerMesh = new OBJMesh(MESHDIR"SR-71_Blackbird.obj");

	glClearColor(0, 0, 0, 1);
}

void Renderer::UpdateScene(float msec)
{
	this->msec = msec;
	total_sec_pass += msec/1000;
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_T)) //Build Cube
	{
		toggleWireFrame();
	}

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_M)) //Build Cube
	{
		render_motion_blur = !render_motion_blur;
	}


	time += msec;

	if (time >= 1000)
	{
		fps = nbFrames;
		time = 0;
		nbFrames = 0;
	}

	nbFrames++;

	if (camera)
	{
		camera->UpdateCamera(msec);
	}
	root->Update(msec);

	//Used bor the cooldown bar
	if (cd > 0)
	{
		cd -= msec;
	}
	
}

void Renderer::RenderScene()
{
	
	if (render_motion_blur){



		RenderMotionBlur();

		//Display HUD
		displayInformation();

		

		SwapBuffers();
	}
	else{
		RenderWithoutPostProcessing();
	}

}

void Renderer::RenderWithoutPostProcessing(){

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	RenderBackground();

	if (camera)
	{


		textureMatrix.ToIdentity();
		modelMatrix.ToIdentity();
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
		frameFrustum.FromMatrix(projMatrix * viewMatrix);
		

		//Return to default 'usable' state every frame!
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glDisable(GL_STENCIL_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		BuildNodeLists(root);
		SortNodeLists();
		DrawNodes();
		ClearNodeLists();

		RenderUI();

		DrawAfterBurner();
		galaxy_system.Render(msec, viewMatrix, projMatrix);
		DrawTornado();
		DrawFire();

		

		//Display HUD
		if (wireFrame)
		{
			toggleWireFrame();
			//Display HUD
			displayInformation();
			toggleWireFrame();

		}
		else
		{
			displayInformation();
		}
	}

	glUseProgram(0);


	SwapBuffers();
}

void Renderer::RenderMotionBlur(){


	//render scene need motion blur and fog
	glBindFramebuffer(GL_FRAMEBUFFER, motion_blur_FBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	if (camera)
	{
		
		textureMatrix.ToIdentity();
		modelMatrix.ToIdentity();
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
		frameFrustum.FromMatrix(projMatrix * viewMatrix);
		

		//Return to default 'usable' state every frame!
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glDisable(GL_STENCIL_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		BuildNodeLists(root);
		SortNodeLists();
		DrawNodes();
		ClearNodeLists();

	}

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//render the particle to a texture firstly
	//this scene store in particle_ColourTex
	RenderParticleToTexture();

	PresentMotionBlur();

}

void Renderer::PresentMotionBlur(){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(motion_blur_shader);

	

	//update current viewMatrix
	current_viewMatrix = viewMatrix;//get current camera view matrix
	float track_speed = PhysicsSystem::GetTrackSpeed();
//	previous_viewMatrix = current_viewMatrix;
	previous_viewMatrix.values[14] = previous_viewMatrix.values[14] - track_speed;
	/*
	current_viewMatrix.values[12] = current_viewMatrix.values[12] / 1000;
	current_viewMatrix.values[13] = current_viewMatrix.values[13] / 1000;
	current_viewMatrix.values[14] = current_viewMatrix.values[14] / 1000;

	previous_viewMatrix = viewMatrix;
	previous_viewMatrix.values[14] = previous_viewMatrix.values[14] - 200;
	*/
	Matrix4 inverse_pv_matrix = Matrix4::InvertMatrix(projMatrix*current_viewMatrix);

	//	previous_viewMatrix.values[14] = previous_viewMatrix.values[14] - 100;

	Matrix4 previous_pv_matrix = projMatrix * previous_viewMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(),
		"previousPVMatrix"), 1, false, (float*)&previous_pv_matrix);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(),
		"inversePVMatrix"), 1, false, (float*)&inverse_pv_matrix);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"depthTex"), 4);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"particleTex"), 5);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"backgroundTex"), 6);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, motion_blur_DepthTex);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, particle_ColourTex);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, background_ColourTex);

	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);

	textureMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();


	quad_motion_blur->SetTexture(motion_blur_ColourTex);
	quad_motion_blur->Draw();

	previous_viewMatrix = current_viewMatrix;

	glUseProgram(0);
}

void Renderer::RenderPauseMenu()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	if (camera)
	{
		SetCurrentShader(simpleShader);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

		textureMatrix.ToIdentity();
		modelMatrix.ToIdentity();
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
		frameFrustum.FromMatrix(projMatrix * viewMatrix);
		UpdateShaderMatrices();

		//Return to default 'usable' state every frame!
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glDisable(GL_STENCIL_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		BuildNodeLists(root);
		SortNodeLists();
		DrawNodes();
		ClearNodeLists();
	}

	//Set Shader to TexturedShader
	SetCurrentShader(menuShader);

	//Reset Matrixes
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	modelMatrix.ToIdentity();
	UpdateShaderMatrices();

	//If in wireframe mode toggle it off
	if (wireFrame)
	{
		toggleWireFrame();
	}

	Vector3 translation(-0.7f, -0.7f, 0.0f);
	Vector3 scale(0.25f, 0.25f, 0.25f);

	modelMatrix = Matrix4::Translation(translation) *  Matrix4::Scale(scale);
	UpdateShaderMatrices();

	for (int x = 0; x < PAUSE_BUTTONS_SIZE; x++)
	{
		pauseMenuButtons[x]->checkAndDraw(x == pauseButtonIndex);
		translation += Vector3(0.0f, 0.25f, 0.0f);
		modelMatrix = Matrix4::Translation(translation) *  Matrix4::Scale(scale);
		UpdateShaderMatrices();
	}

	//------------Added by Sam for controller input---------------------
	float LY = 0;
	XINPUT_STATE controllerState = Window::GetController()->GetState();

	if (Window::GetControllerConnected())
	{
		//-------------------get left stick values----------------------
		LY = controllerState.Gamepad.sThumbLY;
		//check if the controller is outside dead zone
		if (abs(LY) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			//clip the magnitude at its expected maximum value
			if (LY > 32767)
			{
				LY = 32767;
			}
			if (LY < -32767)
			{
				LY = -32767;
			}

			//adjust magnitude relative to the end of the dead zone
			if (LY > 0)
			{
				LY -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
			}
			else //will have to be negative due to previous ifs....so no need for if
			{
				LY += XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
			}
		}
		else //if the controller is in the deadzone zero out the magnitude
		{
			LY = 0.0f;
		}
		//------------------------------------------------------------------
		//----------------------------------------------------------------------
	}

	if (!(Window::GetinputScrollCDing()) && ((Window::GetWindow().GetKeyboard()->KeyDown(KEYBOARD_UP)) || LY > 0))
	{
		if (pauseButtonIndex > 0)
		{
			pauseButtonIndex--;
			Window::enableInputScrollLock();
		}

	}
	else if (!(Window::GetinputScrollCDing()) && ((Window::GetWindow().GetKeyboard()->KeyDown(KEYBOARD_DOWN)) || LY < 0))
	{
		if (pauseButtonIndex < PAUSE_BUTTONS_SIZE - 1)
		{
			pauseButtonIndex++;
			Window::enableInputScrollLock();
		}

	}

	if ((Window::GetWindow().GetKeyboard()->KeyTriggered(KEYBOARD_RETURN)) || controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A)
	{
		switch (pauseButtonIndex)
		{
		case 0:
			GameClass::GetGameClass().setCurrentState(GAME_PLAYING);
			break;

		case 1:
			GameClass::GetGameClass().setCurrentState(GAME_EXIT);
			break;

		default:
			break;
		}
	}

	glUseProgram(0);
	SwapBuffers();
}

void Renderer::RenderUI()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	if (camera)
	{
		SetCurrentShader(simpleShader);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

		textureMatrix.ToIdentity();
		modelMatrix.ToIdentity();
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
		frameFrustum.FromMatrix(projMatrix * viewMatrix);
		UpdateShaderMatrices();

		//Return to default 'usable' state every frame!
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glDisable(GL_STENCIL_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		BuildNodeLists(root);
		SortNodeLists();
		DrawNodes();
		ClearNodeLists();
	}


	//Set Shader to TexturedShader
	SetCurrentShader(menuShader);

	//Reset Matrixes
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	modelMatrix.ToIdentity();
	UpdateShaderMatrices();
	
	//Score
	modelMatrix = Matrix4::Translation(Vector3(-0.82f, -0.88f, 0.0f)) *  Matrix4::Scale(Vector3(0.15f, 0.1f, 0.5f));
	UpdateShaderMatrices();
	uiQuad->Draw();

	//Multiplier
	modelMatrix = Matrix4::Translation(Vector3(-0.48f, -0.88f, 0.0f)) *  Matrix4::Scale(Vector3(0.15f, 0.1f, 0.5f));
	UpdateShaderMatrices();
	uiQuad->Draw();

	//Stack Counter
	modelMatrix = Matrix4::Translation(Vector3(-0.1f, -0.88f, 0.0f)) *  Matrix4::Scale(Vector3(0.15f, 0.1f, 0.5f));
	UpdateShaderMatrices();
	uiQuad->Draw();

	//Timer
	modelMatrix = Matrix4::Translation(Vector3(-0.82f, -0.65f, 0.0f)) *  Matrix4::Scale(Vector3(0.15f, 0.1f, 0.5f));
	UpdateShaderMatrices();
	uiQuad->Draw();

	//Button Y
	modelMatrix = Matrix4::Translation(Vector3(0.62f, -0.85f, 0.0f)) *  Matrix4::Scale(Vector3(0.05f, 0.05f, 0.5f));
	UpdateShaderMatrices();
	buttonY->Draw();

	//Button X
	modelMatrix = Matrix4::Translation(Vector3(0.52f, -0.75f, 0.0f)) *  Matrix4::Scale(Vector3(0.05f, 0.05f, 0.5f));
	UpdateShaderMatrices();
	buttonX->Draw();

	//Button B
	modelMatrix = Matrix4::Translation(Vector3(0.72f, -0.75f, 0.0f)) *  Matrix4::Scale(Vector3(0.05f, 0.05f, 0.5f));
	UpdateShaderMatrices();
	buttonB->Draw();

	
	//Cooldown Bar
	if (Window::GetWindow().GetKeyboard()->KeyDown(KEYBOARD_Q))
	{
		cd = 1000;
	}

	//cd = cd * 0.9;
	modelMatrix = Matrix4::Translation(Vector3(0.0f, 0.7f, 0.0f)) *  Matrix4::Scale(Vector3(cd/1000, 0.05f, 0.5f));
	UpdateShaderMatrices();
	cooldownBar->Draw();

	
}

buttonPressed Renderer::RenderMainMenu()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	/*SetCurrentShader(simpleShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);*/

	textureMatrix.ToIdentity();
	modelMatrix.ToIdentity();
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	frameFrustum.FromMatrix(projMatrix * viewMatrix);
	UpdateShaderMatrices();

	//Return to default 'usable' state every frame!
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Set Shader to TexturedShader
	SetCurrentShader(menuShader);

	//Reset Matrixes
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	modelMatrix.ToIdentity();
	UpdateShaderMatrices();

	//If in wireframe mode toggle it off
	if (wireFrame)
	{
		toggleWireFrame();
	}


	glDisable(GL_DEPTH_TEST);
	mainMenuQuad->Draw();

	Vector3 translation2 = Vector3(-0.6f, 0.47f, 0.0f);
	Vector3 scale2 = Vector3(0.35f, 0.15f, 0.35f);
	modelMatrix = Matrix4::Translation(translation2) *  Matrix4::Scale(scale2);
	UpdateShaderMatrices();

	ipQuad->Draw();


	Vector3 translation(0.0f, -0.6f, 0.0f);
	Vector3 scale(0.35f, 0.35f, 0.35f);
	modelMatrix = Matrix4::Translation(translation) *  Matrix4::Scale(scale);
	UpdateShaderMatrices();

	for (int x = 0; x < MAIN_BUTTONS_SIZE; x++)
	{
		mainMenuButtons[x]->checkAndDraw(x == mainButtonIndex);
		translation += Vector3(0.0f, 0.25f, 0.0f);
		modelMatrix = Matrix4::Translation(translation) *  Matrix4::Scale(scale);
		UpdateShaderMatrices();
	}

	if (Window::GetWindow().GetKeyboard()->KeyTriggered(KEYBOARD_0))
	{
		ipAddress += "0";
	}
	if (Window::GetWindow().GetKeyboard()->KeyTriggered(KEYBOARD_1))
	{
		ipAddress += "1";
	}
	if (Window::GetWindow().GetKeyboard()->KeyTriggered(KEYBOARD_2))
	{
		ipAddress += "2";
	}
	if (Window::GetWindow().GetKeyboard()->KeyTriggered(KEYBOARD_3))
	{
		ipAddress += "3";
	}
	if (Window::GetWindow().GetKeyboard()->KeyTriggered(KEYBOARD_4))
	{
		ipAddress += "4";
	}
	if (Window::GetWindow().GetKeyboard()->KeyTriggered(KEYBOARD_5))
	{
		ipAddress += "5";
	}
	if (Window::GetWindow().GetKeyboard()->KeyTriggered(KEYBOARD_6))
	{
		ipAddress += "6";
	}
	if (Window::GetWindow().GetKeyboard()->KeyTriggered(KEYBOARD_7))
	{
		ipAddress += "7";
	}
	if (Window::GetWindow().GetKeyboard()->KeyTriggered(KEYBOARD_8))
	{
		ipAddress += "8";
	}
	if (Window::GetWindow().GetKeyboard()->KeyTriggered(KEYBOARD_9))
	{
		ipAddress += "9";
	}
	if (Window::GetWindow().GetKeyboard()->KeyTriggered(KEYBOARD_PERIOD))
	{
		ipAddress += ".";
	}
	if (Window::GetWindow().GetKeyboard()->KeyTriggered(KEYBOARD_BACK))
	{
		ipAddress = ipAddress.substr(0, ipAddress.size() - 1);
	}

	if (ipAddress.length() > 15)
	{
		ipAddress = ipAddress.substr(0, 15);
	}

	glDepthMask(false);
	glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);

	DrawText(ipAddress, Vector3(width / 4.0f - ipAddress.length() * 12.0f - 20, height / 2 + 175.0f, 0.0f), 20.0f, false);


	glDepthMask(true);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



	//------------Added by Sam for controller input---------------------
	float LY = 0;
	XINPUT_STATE controllerState = Window::GetController()->GetState();

	if (Window::GetControllerConnected())
	{
		//-------------------get left stick values----------------------
		LY = controllerState.Gamepad.sThumbLY;
		//check if the controller is outside dead zone
		if (abs(LY) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			//clip the magnitude at its expected maximum value
			if (LY > 32767)
			{
				LY = 32767;
			}
			if (LY < -32767)
			{
				LY = -32767;
			}

			//adjust magnitude relative to the end of the dead zone
			if (LY > 0)
			{
				LY -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
			}
			else //will have to be negative due to previous ifs....so no need for if
			{
				LY += XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
			}
		}
		else //if the controller is in the deadzone zero out the magnitude
		{
			LY = 0.0f;
		}
		//------------------------------------------------------------------
		//----------------------------------------------------------------------
	}


	if (!(Window::GetinputScrollCDing()) && ((Window::GetWindow().GetKeyboard()->KeyDown(KEYBOARD_UP)) || LY > 0))
	{
		if (mainButtonIndex > 0)
		{
			Window::enableInputScrollLock();
			mainButtonIndex--;
		}

	}
	else if (!(Window::GetinputScrollCDing()) && ((Window::GetWindow().GetKeyboard()->KeyDown(KEYBOARD_DOWN)) || LY < 0))
	{
		if (mainButtonIndex < MAIN_BUTTONS_SIZE - 1)
		{
			Window::enableInputScrollLock();
			mainButtonIndex++;
		}

	}

	if ((Window::GetWindow().GetKeyboard()->KeyTriggered(KEYBOARD_RETURN)) || controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A)
	{
		switch (mainButtonIndex)
		{
		case 0:
			return SOLO;
			break;

		case 1:
			return HOST_2;
			break;

		case 2:
			return HOST_3;
			break;

		case 3:
			return HOST_4;
			break;
		case 4:
			return CLIENT;
			break;
		case 5:
			return EXIT;
			break;

		default:
			return NO_PRESSED;
			break;
		}
	}

	glUseProgram(0);
	SwapBuffers();

	return NO_PRESSED;
}

void Renderer::RenderLoading(int percent, string message)
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//Return to default 'usable' state every frame!

	glEnable(GL_CULL_FACE);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Set Shader to TexturedShader
	SetCurrentShader(menuShader);

	//Reset Matrixes
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	modelMatrix.ToIdentity();
	UpdateShaderMatrices();


	//Display HUD
	if (wireFrame)
	{
		toggleWireFrame();
	}

	cout << message << endl;
	//Draw Background Image
	quad->Draw();
	string a = "Loading..." + std::to_string(percent) + "/100";
	cout << a << endl;

	glDepthMask(false);
	glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);


	DrawText(a, Vector3(width / 2.0f - a.length() * 15.0f, height / 2 - 60.0f, 0.0f), 30.0f, false);
	DrawText(message, Vector3(width / 2.0f - message.length() * 15.0f, height / 2 - 30.0f, 0.0f), 30.0f, false);

	glDepthMask(true);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glUseProgram(0);
	SwapBuffers();

	//Delay to read load screen :P
	//for (int x = 0; x < 10000; x++)
	//{
	//	for (int y = 0; y < 100000; y++)
	//	{

	//	}
	//}
}

void	Renderer::DrawNode(SceneNode*n)
{
	RenderType rt = n->GetRenderType();
	switch (rt){
	case WATER_PLANE:{
		if (n->GetMesh())
		{
			SetCurrentShader(water_plane_shader);
			glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "displaceStrength"), 0.3);
			glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "time"), total_sec_pass);
			glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
			UpdateShaderMatrices();

			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&(n->GetWorldTransform()*Matrix4::Scale(n->GetModelScale())));
			glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());
			if (n->GetMesh()->GetTexture() != NULL)
				glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), 1);
			else
				glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), 0);

			n->Draw(*this);

			glUseProgram(0);

		}
		break;
	}
	default:{
		if (n->GetMesh())
		{
			SetCurrentShader(simpleShader);
			glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
			UpdateShaderMatrices();

			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&(n->GetWorldTransform()*Matrix4::Scale(n->GetModelScale())));
			glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());
			if (n->GetMesh()->GetTexture() != NULL)
				glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), 1);
			else
				glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), 0);

			n->Draw(*this);
			glUseProgram(0);
		}
	}
	}
	
}

void	Renderer::BuildNodeLists(SceneNode* from)
{
	Vector3 direction = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
	from->SetCameraDistance(Vector3::Dot(direction, direction));

	if (frameFrustum.InsideFrustum(*from))
	{
		if (from->GetColour().w < 1.0f)
		{
			transparentNodeList.push_back(from);
		}
		else
		{
			nodeList.push_back(from);
		}
	}

	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i)
	{
		BuildNodeLists((*i));
	}
}

void	Renderer::DrawNodes()
{
	for (vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); ++i) {
		DrawNode((*i));
	}

	for (vector<SceneNode*>::const_reverse_iterator i = transparentNodeList.rbegin(); i != transparentNodeList.rend(); ++i)
	{
		DrawNode((*i));
	}
}

void	Renderer::SortNodeLists()
{
	std::sort(transparentNodeList.begin(), transparentNodeList.end(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void	Renderer::ClearNodeLists()	{
	transparentNodeList.clear();
	nodeList.clear();
}

void	Renderer::SetCamera(ChaseCamera*c)
{
	camera = c;
}

void	Renderer::AddNode(SceneNode* n)
{
	nodeCount++;
	root->AddChild(n);
}

void	Renderer::RemoveNode(SceneNode* n)
{
	nodeCount--;
	root->RemoveChild(n);
}

void Renderer::DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective)
{

	//Create a new temporary TextMesh, using our line of text andour font
	TextMesh* mesh = new TextMesh(text, *basicFont);


	//In ortho mode, we subtract the y from the height, so that a height of 0 is at the top left of the screen, which is more intuitive
	modelMatrix = Matrix4::Translation(Vector3(position.x, height - position.y, position.z)) * Matrix4::Scale(Vector3(size, size, 1));
	viewMatrix.ToIdentity();
	projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();

	mesh->Draw();

	delete mesh; //Once it's drawn, we don't need it anymore!


}

void Renderer::displayInformation()
{
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	SetCurrentShader(textShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	Matrix4 viewMatrixTemp = viewMatrix;
	Matrix4 projMatrixTemp = projMatrix;
	Matrix4 modelMatrixTemp = modelMatrix;
	int x = 64;
	/*DrawText("Physics FPS: " + to_string(PhysicsSystem::getFPS()), Vector3(0, 16, 0), 16, false);
	DrawText("Renderer FPS: " + to_string(fps), Vector3(0, 32, 0), 16, false);
	DrawText("Number of Nodes: " + to_string(nodeCount), Vector3(0, 48, 0), 16, false);

	DrawText("Controls:", Vector3(0, 112, 0), 16, false);
	DrawText("T = Toggle Wireframe", Vector3(0, 128, 0), 16, false);
	DrawText("WASD = Move", Vector3(0, 144, 0), 16, false);
	DrawText("Shift = Down", Vector3(0, 160, 0), 16, false);
	DrawText("Space = Up", Vector3(0, 176, 0), 16, false);*/
	
	//--------------------------------------------------GUI--------------------------------------------------------------------------------
	DrawText("SCORE", Vector3(25, 15, 0), 25, false);
	DrawText("1250", Vector3(32, 50, 0), 25, false);

	DrawText("SCORE", Vector3(195, 15, 0), 25, false);
	DrawText("1250", Vector3(205, 50, 0), 25, false);

	DrawText("SCORE", Vector3(375, 15, 0), 25, false);
	DrawText(to_string(cd), Vector3(395, 50, 0), 25, false);

	DrawText("Timer", Vector3(25, 105, 0), 25, false);
	DrawText(to_string(PhysicsSystem::GetPhysicsSystem().GetCheckPointTimer()), Vector3(32, 135, 0), 25, false);

	DrawText("Immune", Vector3(740, 15, 0), 20, false);
	DrawText("Slow", Vector3(640, 85, 0), 20, false);
	DrawText("No CD", Vector3(880, 85, 0), 20, false);

	//--------------------------------------------------GUI--------------------------------------------------------------------------------

	//DrawText("(R)Reset Boxes", Vector3(0, 192, 0), 16, false);

	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(0);

	viewMatrix = viewMatrixTemp;
	projMatrix = projMatrixTemp;
	modelMatrix = modelMatrixTemp;
}

void Renderer::toggleWireFrame()
{
	wireFrame = !wireFrame;

	if (wireFrame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

bool Renderer::CreatMotionBlurBuffer(){
	// Generate our scene depth texture ...
	glGenTextures(1, &motion_blur_DepthTex);
	glBindTexture(GL_TEXTURE_2D, motion_blur_DepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height,
		0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	// And our colour texture ...
	glGenTextures(1, &motion_blur_ColourTex);
	glBindTexture(GL_TEXTURE_2D, motion_blur_ColourTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, NULL);


	glGenFramebuffers(1, &motion_blur_FBO);


	glBindFramebuffer(GL_FRAMEBUFFER, motion_blur_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, motion_blur_DepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
		GL_TEXTURE_2D, motion_blur_DepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, motion_blur_ColourTex, 0);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ||
		!motion_blur_DepthTex || !motion_blur_ColourTex) {
		//check FBO attachment success using this command
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

void Renderer::DrawAfterBurner(){
	Matrix4 model_matrix = spaceship_scene_node->GetAfterburnerNode()->GetWorldTransform();
	if (camera){

		viewMatrix = camera->BuildViewMatrix();
	}

	spaceship_scene_node->afterburner_system[0].Render(msec, model_matrix, projMatrix, viewMatrix);
	spaceship_scene_node->afterburner_system[1].Render(msec, model_matrix, projMatrix, viewMatrix);
}

void Renderer::DrawTornado(){
	for (vector<TornadoSceneNode*>::iterator i = tornadoNode.begin(); i != tornadoNode.end(); ++i)
	{
		Matrix4 model_matrix2 = (*i)->GetTornadoNode()->GetWorldTransform();

		//Matrix4 model_matrix = spaceship_scene_node->GetAfterburnerNode()->GetWorldTransform();
		//viewMatrix.ToIdentity();
		if (camera){

			viewMatrix = camera->BuildViewMatrix();
		}



		tornado_system.Render(msec, model_matrix2, projMatrix, viewMatrix);

	}



}

void Renderer::DrawFire(){
	for (vector<FireSceneNode*>::iterator i = fireNode.begin(); i != fireNode.end(); ++i)
	{
		Matrix4 model_matrix = (*i)->GetFireNode()->GetWorldTransform();

		//Matrix4 model_matrix = spaceship_scene_node->GetAfterburnerNode()->GetWorldTransform();
		//viewMatrix.ToIdentity();
		if (camera){

			viewMatrix = camera->BuildViewMatrix();
		}



		fire_system.Render(msec, model_matrix, projMatrix, viewMatrix);

	}
}

bool Renderer::CreatParticleBuffer(){

	//colour texture ...
	glGenTextures(1, &particle_ColourTex);
	glBindTexture(GL_TEXTURE_2D, particle_ColourTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, NULL);


	glGenTextures(1, &background_ColourTex);
	glBindTexture(GL_TEXTURE_2D, background_ColourTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glGenFramebuffers(1, &particle_FBO);


	glBindFramebuffer(GL_FRAMEBUFFER, particle_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, particle_ColourTex, 0);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ||
		!particle_ColourTex) {
		//check FBO attachment success using this command
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

void Renderer::RenderParticleToTexture(){
	glBindFramebuffer(GL_FRAMEBUFFER, particle_FBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, background_ColourTex, 0);
	RenderBackground();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, particle_ColourTex, 0);
	galaxy_system.Render(msec, viewMatrix, projMatrix);
	DrawAfterBurner();
	
	DrawTornado();
	DrawFire();
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::RenderBackground(){
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	SetCurrentShader(backgroundShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	PushMatrix();
	MatrixToIdentity();
	//	projMatrix = Matrix4::Perspective(0.5f, 10000.0f, (float)width / (float)height, 45.0f);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	
	Matrix4 pushPos = Matrix4::Translation(Vector3(0.5f, 0.5f, 0));
	Matrix4 popPos = Matrix4::Translation(Vector3(-0.5f, -0.5f, 0));
	Matrix4 rotation = camera->BuildViewMatrix();
	rotation.values[12] = 0;
	rotation.values[13] = 0;
	rotation.values[14] = 0;

	textureMatrix = pushPos * rotation *Matrix4::Scale(Vector3(0.7, 0.7, 0.7))* popPos;

	UpdateShaderMatrices();
	quad->SetTexture(background[0]);
	quad->Draw();

	PopMatrix();
	glUseProgram(0);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::PushMatrix(){
	modeltemp = modelMatrix;
	viewtemp = viewMatrix;
	projtemp = projMatrix;
	texturetemp = textureMatrix;
}

void Renderer::PopMatrix(){
	modelMatrix = modeltemp;
	viewMatrix = viewtemp;
	projMatrix = projtemp;
	textureMatrix = texturetemp;
}

void Renderer::MatrixToIdentity(){
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	textureMatrix.ToIdentity();
}