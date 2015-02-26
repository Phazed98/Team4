#include "Renderer.h"

Renderer* Renderer::instance = NULL;

Renderer::Renderer(Window &parent) : OGLRenderer(parent)
{
	camera = NULL;

	root = new SceneNode();

	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	//add by steven for motion blur
	quad_motion_blur = Mesh::GenerateQuad();
	render_motion_blur = false; 
	motion_blur_shader = new Shader(SHADERDIR"MotionBlurVertex.glsl", SHADERDIR"MotionBlurFragment.glsl");
	
	simpleShader = new Shader(SHADERDIR"TechVertex.glsl", SHADERDIR"TechFragment.glsl");
	textShader = new Shader(SHADERDIR"TexVertex.glsl", SHADERDIR"TexFragment.glsl");
	texturedShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	menuShader = new Shader(SHADERDIR"menuVertex.glsl", SHADERDIR"menuFragment.glsl");

	quad = Mesh::GenerateQuad();
	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Loading Screen.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	quad->SetColour(new Vector4(1, 0, 0, 1));

	playButton = new Button(Mesh::GenerateQuad(), Vector2(30, 50), Vector2(376, 178));
	playButton->SetDefaultTexture(SOIL_load_OGL_texture(TEXTUREDIR"resume.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	playButton->SetHighlightTexture(SOIL_load_OGL_texture(TEXTUREDIR"resumeHighlighted.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	playButton->SetPressedTexture(SOIL_load_OGL_texture(TEXTUREDIR"resumeClicked.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	//resetButton = new Button(Mesh::GenerateQuad(), Vector2(100,150), Vector2(200,300));
	//resetButton->getMesh()->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Barren Grey.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	exitButton = new Button(Mesh::GenerateQuad(), Vector2(30, 200), Vector2(376, 350));
	exitButton->SetDefaultTexture(SOIL_load_OGL_texture(TEXTUREDIR"ExitButton.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	exitButton->SetHighlightTexture(SOIL_load_OGL_texture(TEXTUREDIR"ExitButtonHighlighted.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	exitButton->SetPressedTexture(SOIL_load_OGL_texture(TEXTUREDIR"ExitButtonPressed.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));



	if (!quad->GetTexture()){
		cout << "error in loading screen texture!!" << endl;
		return;
	}

	if (!simpleShader->LinkProgram() || !textShader->LinkProgram() ||
		!texturedShader->LinkProgram() || !menuShader->LinkProgram() || 
		!motion_blur_shader->LinkProgram())
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

void Renderer::UpdateScene(float msec)
{
	this->msec = msec;
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

		DrawAfterBurner();

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
	glBindFramebuffer(GL_FRAMEBUFFER, motion_blur_FBO);
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

		DrawAfterBurner();
	}

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	PresentMotionBlur();

}
void Renderer::PresentMotionBlur(){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(motion_blur_shader);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"depthTex"), 4);


	//update current viewMatrix
	current_viewMatrix = viewMatrix;//get current camera view matrix
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
	previous_viewMatrix = current_viewMatrix;


	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, motion_blur_DepthTex);


	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);

	textureMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();


	quad_motion_blur->SetTexture(motion_blur_ColourTex);
	quad_motion_blur->Draw();


	glUseProgram(0);
}

void Renderer::RenderMenu()
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


	//Display HUD
	if (wireFrame)
	{
		toggleWireFrame();
	}

	//Draw Background Image
	//quad->Draw();


	//X = -1 to 1 on resolution 1280,800 -1 = 0, 1 = 1280
	//Y = -1 to 1 on resolution 1280,800 -1 = 0, 1 = 800

	Vector2 mousePos = Window::GetWindow().GetMouse()->GetAbsolutePosition();
	bool mousePressed = Window::GetWindow().GetMouse()->ButtonDown(MOUSE_LEFT);

	float correctedX = mousePos.x + -0.7 * 1280;
	float correctedY = mousePos.y + -0.7 * 800;

	//cout << mousePos.x << ", " << mousePos.y << endl;

	modelMatrix = Matrix4::Translation(Vector3(-0.7, -0.7, 0)) *  Matrix4::Scale(Vector3(0.25, 0.25, 0.25));
	UpdateShaderMatrices();
	if (playButton->checkAndDraw(mousePos, mousePressed))
	{
		GameClass::GetGameClass().setCurrentState(GAME_PLAYING);
	}

	//modelMatrix = Matrix4::Translation(Vector3(-0.7, -0.3, 0)) *  Matrix4::Scale(Vector3(0.25, 0.15, 100.25));
	//UpdateShaderMatrices();
	//resetButton->Draw(mousePos);

	modelMatrix = Matrix4::Translation(Vector3(-0.7, -0.3, 0)) *  Matrix4::Scale(Vector3(0.25, 0.25, 0.25));
	UpdateShaderMatrices();
	if (exitButton->checkAndDraw(mousePos, mousePressed))
	{
		GameClass::GetGameClass().setCurrentState(GAME_EXIT);
	}

	glUseProgram(0);
	SwapBuffers();
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


	DrawText(a, Vector3(width / 2 - a.length() * 15, height / 2 - 60, 0), 30, false);
	DrawText(message, Vector3(width / 2 - message.length() * 15, height / 2 - 30, 0), 30, false);

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
	if (n->GetMesh())
	{
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&(n->GetWorldTransform()*Matrix4::Scale(n->GetModelScale())));
		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());
		if (n->GetMesh()->GetTexture() != NULL)
			glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), 1);
		else
			glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), 0);

		n->Draw(*this);
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

void	Renderer::SetCamera(Camera*c)
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

	DrawText("Physics FPS: " + to_string(PhysicsSystem::getFPS()), Vector3(0, 16, 0), 16, false);
	DrawText("Renderer FPS: " + to_string(fps), Vector3(0, 32, 0), 16, false);
	DrawText("Number of Nodes: " + to_string(nodeCount), Vector3(0, 48, 0), 16, false);

	DrawText("Controls:", Vector3(0, 112, 0), 16, false);
	DrawText("T = Toggle Wireframe", Vector3(0, 128, 0), 16, false);
	DrawText("WASD = Move", Vector3(0, 144, 0), 16, false);
	DrawText("Shift = Down", Vector3(0, 160, 0), 16, false);
	DrawText("Space = Up", Vector3(0, 176, 0), 16, false);
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
//	Matrix4 model_matrix = Matrix4::Translation(Vector3(0,0,0));
	if (camera){

		viewMatrix = camera->BuildViewMatrix();
	}
	spaceship_scene_node->afterburner_system[0].Render(msec, model_matrix, projMatrix, viewMatrix);
	spaceship_scene_node->afterburner_system[1].Render(msec, model_matrix, projMatrix, viewMatrix);
}

