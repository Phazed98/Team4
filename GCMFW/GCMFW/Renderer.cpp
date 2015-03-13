#include "Renderer.h"
#include "Input.h"
#include "MyGame.h"
#include <string>
#include <sstream>

Renderer* Renderer::instance = NULL;

Renderer::Renderer(void)	
{
	/*
	You're provided with a very basic vertex / fragment shader, to get you started
	with Cg, and drawing textured objects.
	*/
	VertexShader* basicVert = new VertexShader("/vertex.vpo");
	FragmentShader* basicFrag = new FragmentShader("/fragment.fpo");

	this->SetCurrentShader(*basicVert, *basicFrag);
	



	score = 0; 
	timer = 20;

	/*
	Projection matrix...0.7853982 is 45 degrees in radians.
	*/
	projMatrix = Matrix4::perspective(0.7853982, screenRatio, 1.0f, 20000.0f);	//CHANGED TO THIS!!

	nodeCount = 0;
	nbFrames = 0;
	time = 0;						//Float variable to hold milliseconds, counts to 1000 useing msec to attain framerate
	fps = 0;				//FPS string to use with DrawText
	wireFrame = false;
	coolDown = 0;

	std::cout << "New Root Has Been Made!!!!!" << std::endl;
	root = new SceneNode();

	basicFont = new Font(*GCMRenderer::LoadGTF("/Textures/Tahoma.gtf"), 16, 16);

	pauseButtonIndex = 0;
	mainButtonIndex = 0;

	GameOverQuad = Mesh::GenerateQuad();
	GameOverQuad->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/Stars.gtf"));
	GameOverSN = new SceneNode(GameOverQuad);

	BackGrounMesh = Mesh::GenerateQuad();
	BackGrounMesh->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/Stars.gtf"));
	BackGroundSceneNode = new SceneNode(BackGrounMesh);

	mainMenuQuad = Mesh::GenerateQuad();
	mainMenuQuad->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/Loading.gtf"));
	mainMenuSceneNode = new SceneNode(mainMenuQuad);

	selectorMesh = Mesh::GenerateTriangle();
	selectorMesh->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/Black.gtf"));
	selectorSceneNode = new SceneNode(selectorMesh);
	selectorSceneNode->SetModelScale(Vector3(0.4, 0.8, 0.8));

	//Main Menu Buttons

	//Solo
	Mesh* mainButtonMesh = Mesh::GenerateQuad();
	mainButtonMesh->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/Solo.gtf"));
	mainMenuButtons[0] = new SceneNode(mainButtonMesh);
	mainMenuButtons[0]->SetTransform(Matrix4::translation(Vector3(0.0f, -0.5f, 0)) * Matrix4::scale(Vector3(0.2f, 0.1f, 0.1f)));
	mainMenuButtons[0]->Update(0);
	

	//Split Screen
	Mesh* mainButtonMesh2 = Mesh::GenerateQuad();
	mainButtonMesh2->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/Split.gtf"));
	mainMenuButtons[1] = new SceneNode(mainButtonMesh2);
	mainMenuButtons[1]->SetTransform(Matrix4::translation(Vector3(0.0f, -0.2f, 0)) * Matrix4::scale(Vector3(0.2f, 0.1f, 0.1f)));
	mainMenuButtons[1]->Update(0);


	//Exit
	Mesh* mainButtonMesh3 = Mesh::GenerateQuad();
	mainButtonMesh3->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/Exit.gtf"));
	mainMenuButtons[2] = new SceneNode(mainButtonMesh3);
	mainMenuButtons[2]->SetTransform(Matrix4::translation(Vector3(0.0f, 0.1f, 0)) * Matrix4::scale(Vector3(0.2f, 0.1f, 0.1f)));
	mainMenuButtons[2]->Update(0);

	//Pause Menu Buttons

	//Resume
	Mesh* pauseButtonMesh = Mesh::GenerateQuad();
	pauseButtonMesh->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/Play.gtf"));
	pauseMenuButtons[0] = new SceneNode(pauseButtonMesh);
	pauseMenuButtons[0]->SetTransform(Matrix4::translation(Vector3(-0.6f, -0.7f, 0)) * Matrix4::scale(Vector3(0.2f, 0.1f, 0.1f)));
	pauseMenuButtons[0]->Update(0);
	
	//Exit
	Mesh* pauseButtonMesh2 = Mesh::GenerateQuad();
	pauseButtonMesh2->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/Exit.gtf"));
	pauseMenuButtons[1] = new SceneNode(pauseButtonMesh2);
	pauseMenuButtons[1]->SetTransform(Matrix4::translation(Vector3(-0.6f, -0.4f, 0)) * Matrix4::scale(Vector3(0.2f, 0.1f, 0.1f)));
	pauseMenuButtons[1]->Update(0);


	//--------------------------------------------------GUI--------------------------------------------------------------------------------
	scoreQuad = Mesh::GenerateQuad();
	scoreQuad->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/GUITextures/score.gtf"));

	timerQuad = Mesh::GenerateQuad();
	timerQuad->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/GUITextures/score.gtf"));

	cooldownBar = Mesh::GenerateQuad();
	cooldownBar->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/GUITextures/cd.gtf"));

	buttonB = Mesh::GenerateQuad();
	buttonB->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/GUITextures/bigB.gtf"));

	buttonX = Mesh::GenerateQuad();
	buttonX->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/GUITextures/bigX.gtf"));

	buttonY = Mesh::GenerateQuad();
	buttonY->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/GUITextures/bigY.gtf"));

	scoreSN = new SceneNode(scoreQuad);
	modelMatrix = Matrix4::translation(Vector3(-0.82f, -0.88f, 0.0f));
	scoreSN->SetTransform(modelMatrix);
	scoreSN->SetModelScale(Vector3(0.15f, 0.1f, 0.5f));

	timerSN = new SceneNode(timerQuad);
	modelMatrix = Matrix4::translation(Vector3(-0.48f, -0.88f, 0.0f));
	timerSN->SetTransform(modelMatrix);
	timerSN->SetModelScale(Vector3(0.15f, 0.1f, 0.5f));

	cooldownBarSN = new SceneNode(cooldownBar);
	modelMatrix = Matrix4::translation(Vector3(-0.1f, -0.88f, 0.0f));
	cooldownBarSN->SetTransform(modelMatrix);
	cooldownBarSN->SetModelScale(Vector3(0.15f, 0.1f, 0.5f));

	buttonBSN = new SceneNode(buttonB);
	modelMatrix = Matrix4::translation(Vector3(0.72f, -0.75f, 0.0f));
	buttonBSN->SetTransform(modelMatrix);
	buttonBSN->SetModelScale(Vector3(0.15f, 0.1f, 0.5f));


	buttonXSN = new SceneNode(buttonX);
	modelMatrix = Matrix4::translation(Vector3(0.52f, -0.75f, 0.0f));
	buttonXSN->SetTransform(modelMatrix);
	buttonXSN->SetModelScale(Vector3(0.15f, 0.1f, 0.5f));

	buttonYSN = new SceneNode(buttonY);
	modelMatrix = Matrix4::translation(Vector3(0.62f,-0.85f, 0.0f));
	buttonYSN->SetTransform(modelMatrix);
	buttonYSN->SetModelScale(Vector3(0.15f, 0.1f, 0.5f));
	//--------------------------------------------------GUI--------------------------------------------------------------------------------



	instance = this;
	init = true;
}

Renderer::~Renderer(void)	
{
}

void Renderer::UpdateScene(float msec)
{
	//std::cout << "Updating Scene" << std::endl;

	if (coolDown > 0)
		coolDown--;

	time += msec;
	if (time >= 1000)
	{
		fps = nbFrames;
		time = 0;
		nbFrames = 0;

		score = PhysicsSystem::GetPhysicsSystem().GetScore();
		timer = PhysicsSystem::GetPhysicsSystem().GetCheckPointTimer();

		if (timer < 0)
		{
			MyGame::GetGameClass().setCurrentState(GAME_OVER);
		}
	}

	nbFrames++;

	if (camera)
	{
		camera->Update(msec);
	}
	root->Update(msec);
}



/*
Main rendering function. Note how it's essentially the same as the
ones you were writing in OpenGL! We start by clearing the buffer,
render some stuff, then swap the buffers. All that's different is
some slightly different matrix access.

*/
void Renderer::RenderScene() 
{
	//std::cout << "Rendering Scene" << std::endl;
	SetViewport();
	ClearBuffer();
	this->SetCurrentShader(*currentVert, *currentFrag);

	cellGcmSetDepthTestEnable(CELL_GCM_FALSE);
	cellGcmSetDepthFunc(CELL_GCM_NEVER);

	projMatrix = Matrix4::orthographic(-1, 1, 1, -1, -1, 1);
	textureMatrix = Matrix4::identity();
	viewMatrix = Matrix4::identity();
	currentVert->UpdateShaderMatrices(modelMatrix, viewMatrix, projMatrix);

	DrawNode(BackGroundSceneNode);

	cellGcmSetDepthTestEnable(CELL_GCM_TRUE);
	cellGcmSetDepthFunc(CELL_GCM_LESS);

	modelMatrix = Matrix4::identity();
	projMatrix = Matrix4::perspective(0.7853982, screenRatio, 1.0f, 20000.0f);	//CHANGED TO THIS!!

	if (camera) 
	{
		viewMatrix = camera->BuildViewMatrix();
	}
	else
	{
		viewMatrix = Matrix4::identity();
	}

	currentVert->UpdateShaderMatrices(modelMatrix, viewMatrix, projMatrix);

	cellGcmSetDepthTestEnable(CELL_GCM_TRUE);
	cellGcmSetDepthFunc(CELL_GCM_LESS);

	BuildNodeLists(root);
	SortNodeLists();
	DrawNodes();
	ClearNodeLists();

	cellGcmSetDepthTestEnable(CELL_GCM_FALSE);
	cellGcmSetDepthFunc(CELL_GCM_NEVER);

	viewMatrix = Matrix4::identity();
	projMatrix = Matrix4::orthographic(-1, 1, 1, -1, -1, 1);
	//projMatrix = Matrix4::orthographic(0.0f, (float)screenWidth, 0.0f, (float)screenHeight, -1.0f, 1.0f);
	textureMatrix = Matrix4::identity();
	currentVert->UpdateShaderMatrices(modelMatrix, viewMatrix, projMatrix);

	scoreSN->Update(0);
	DrawNode(scoreSN);
	timerSN->Update(0);
	DrawNode(timerSN);
	//cooldownBarSN->Update(0);
	//DrawNode(cooldownBarSN);
	//buttonBSN->Update(0);
	//DrawNode(buttonBSN);
	//buttonXSN->Update(0);
	//DrawNode(buttonXSN);
	//buttonYSN->Update(0);
	//DrawNode(buttonYSN);

	displayInformation();

	cellGcmSetDepthTestEnable(CELL_GCM_TRUE);
	cellGcmSetDepthFunc(CELL_GCM_LESS);

	//projMatrix = Matrix4::perspective(0.7853982, screenRatio, 1.0f, 20000.0f);	//CHANGED TO THIS!!


	SwapBuffers();
}

void	Renderer::RenderPauseMenu()
{
	if (coolDown > 0)
		coolDown--;

	SetViewport();
	ClearBuffer();

	this->SetCurrentShader(*currentVert, *currentFrag);

	cellGcmSetDepthTestEnable(CELL_GCM_FALSE);
	cellGcmSetDepthFunc(CELL_GCM_NEVER);

	projMatrix = Matrix4::orthographic(-1, 1, 1, -1, -1, 1);
	textureMatrix = Matrix4::identity();
	viewMatrix = Matrix4::identity();
	currentVert->UpdateShaderMatrices(modelMatrix, viewMatrix, projMatrix);

	DrawNode(BackGroundSceneNode);


	cellGcmSetDepthTestEnable(CELL_GCM_TRUE);
	cellGcmSetDepthFunc(CELL_GCM_LESS);

	if (camera)
	{
		cellGcmSetDepthTestEnable(CELL_GCM_TRUE);
		cellGcmSetDepthFunc(CELL_GCM_LESS);

		modelMatrix = Matrix4::identity();
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::perspective(0.7853982, screenRatio, 1.0f, 20000.0f);	//CHANGED TO THIS!!
		currentVert->UpdateShaderMatrices(modelMatrix, viewMatrix, projMatrix);

		BuildNodeLists(root);
		SortNodeLists();
		DrawNodes();
		ClearNodeLists();
	}

	
	
	projMatrix = Matrix4::orthographic(-1, 1, 1, -1, -1, 1);

	
	textureMatrix = Matrix4::identity();
	viewMatrix = Matrix4::identity();

	Vector3 translation(-0.7f, -0.7f, 0.0f);
	Vector3 scale(0.25f, 0.25f, 0.25f);

	modelMatrix = Matrix4::translation(translation) *  Matrix4::scale(scale);
	currentVert->UpdateShaderMatrices(modelMatrix, viewMatrix, projMatrix);

	if (Input::ButtonDown(INPUT_UP))
	{
		if (pauseButtonIndex > 0 && coolDown == 0)
		{
			pauseButtonIndex--;
			coolDown = 10;
		}
	}

	else if (Input::ButtonDown(INPUT_DOWN))
	{
		if (pauseButtonIndex < PAUSE_BUTTONS_SIZE - 1 && coolDown == 0)
		{
			pauseButtonIndex++;
			coolDown = 10;
		}
	}


	

	for (int x = 0; x < PAUSE_BUTTONS_SIZE; x++)
	{
		DrawNode(pauseMenuButtons[x]); 

		if (pauseButtonIndex == x)
		{
			selectorSceneNode->SetTransform(pauseMenuButtons[x]->GetWorldTransform() * Matrix4::translation(Vector3(1.5, 0, 0)));
			selectorSceneNode->Update(0);
			DrawNode(selectorSceneNode);
		}

	}

	if (Input::ButtonDown(INPUT_CROSS))
	{
		switch (pauseButtonIndex)
		{
		case 0:
			GameClass::GetGameClass().setCurrentState(GAME_PLAYING);
			break;

		case 1:
			GameClass::GetGameClass().setCurrentState(GAME_EXIT);
			exit(0);
			break;
				
		default:
			break;
		}
	}


	SwapBuffers();
}

buttonPressed	Renderer::RenderMainMenu()
{
	buttonPressed bp = NO_PRESSED;

	if (coolDown > 0)
		coolDown--;

	SetViewport();
	ClearBuffer();

	this->SetCurrentShader(*currentVert, *currentFrag);

	projMatrix = Matrix4::orthographic(-1, 1, 1, -1, -1, 1);
	textureMatrix = Matrix4::identity();
	viewMatrix = Matrix4::identity();
	currentVert->UpdateShaderMatrices(modelMatrix, viewMatrix, projMatrix);

	DrawNode(mainMenuSceneNode);


	if (Input::ButtonDown(INPUT_UP))
	{
		if (mainButtonIndex > 0 && coolDown == 0)
		{
			mainButtonIndex--;
			coolDown = 10;
		}
	}

	else if (Input::ButtonDown(INPUT_DOWN))
	{
		if (mainButtonIndex < MAIN_BUTTONS_SIZE - 1 && coolDown == 0)
		{
			mainButtonIndex++;
			coolDown = 10;
		}
	}

	for (int x = 0; x < MAIN_BUTTONS_SIZE; x++)
	{
		DrawNode(mainMenuButtons[x]);

		if (mainButtonIndex == x)
		{
			selectorSceneNode->SetTransform(mainMenuButtons[x]->GetWorldTransform() * Matrix4::translation(Vector3(1.5, 0, 0)));
			selectorSceneNode->Update(0);
			DrawNode(selectorSceneNode);
		}

	}

	if (Input::ButtonDown(INPUT_CROSS))
	{
		switch (mainButtonIndex)
		{
		case 0:
			bp = SOLO;
			break;

		case 1:
			bp = SPLIT;
			break;

		case 2:
			bp = EXIT;

		default:
			break;
		}
	}


	SwapBuffers();

	return bp;
}


void	Renderer::RenderGameOver()
{

	SetViewport();
	ClearBuffer();

	this->SetCurrentShader(*currentVert, *currentFrag);

	projMatrix = Matrix4::orthographic(-1, 1, 1, -1, -1, 1);
	textureMatrix = Matrix4::identity();
	viewMatrix = Matrix4::identity();
	currentVert->UpdateShaderMatrices(modelMatrix, viewMatrix, projMatrix);

	DrawNode(GameOverSN);

	displayInformation();

	SwapBuffers();
}

void	Renderer::SetCamera(ChaseCamera*c)
{
	camera = c;
}

//void	Renderer::SetCamera(Camera*c)
//{
//	camera = c;
//}

void	Renderer::BuildNodeLists(SceneNode* from)
{
	Vector3 direction = from->GetWorldTransform().getTranslation() - camera->GetPosition();
	from->SetCameraDistance(dot(direction, direction));

//	if (frameFrustum.InsideFrustum(*from))
//	{
		if (from->GetColour().getW()< 1.0f)
		{
			transparentNodeList.push_back(from);
		}
		else
		{
			nodeList.push_back(from);
		}
//	}

	for (std::vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i)
	{
		BuildNodeLists((*i));
	}
}

void	Renderer::DrawNodes()
{
	for (std::vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); ++i) 
	{
		DrawNode((*i));
	}

	for (std::vector<SceneNode*>::const_iterator i = transparentNodeList.end(); i != transparentNodeList.begin(); --i)
	{
		DrawNode((*i));
	}
}

//void	Renderer::DrawNode(SceneNode*n)
//{
//	std::cout << " New Drawnode" <<std::endl;
//	if (n->GetMesh())
//	{
//		Matrix4 transform = n->GetWorldTransform();
//		currentVert->SetParameter("modelMat", transform);
//
//		/*
//		If your SceneNodes can have their own texture, handle the setting of it
//		here! In Graphics for Games, the Mesh::Draw function handled the setting
//		of textures to units, but the slight changes in how GCM handles textures
//		make it more intuitive to place it here, instead.
//		*/
//
//		SetTextureSampler(currentFrag->GetParameter("texture"), n->GetMesh()->GetDefaultTexture());
//
//		/*
//		The GCM Mesh class needs the current vertex shader, fragment
//		shader is just sent for convenience, in case it's needed in future...
//		*/
//		n->GetMesh()->Draw(*currentVert, *currentFrag);
//	}
//}

void	Renderer::SortNodeLists()
{
	std::sort(transparentNodeList.begin(), transparentNodeList.end(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void	Renderer::ClearNodeLists()	
{
	transparentNodeList.clear();
	nodeList.clear();
}

void	Renderer::AddNode(SceneNode* n)
{
	//nodeCount++;
	root->AddChild(n);
}

void	Renderer::RemoveNode(SceneNode* n)
{
	//nodeCount--;
	root->RemoveChild(n);
}

void Renderer::DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective)
{
	//Create a new temporary TextMesh, using our line of text andour font
	TextMesh* mesh = new TextMesh(text, *basicFont);
	textNode = new SceneNode(mesh);


	//In ortho mode, we subtract the y from the height, so that a height of 0 is at the top left of the screen, which is more intuitive
	//modelMatrix = Matrix4::translation(Vector3(position.getX(), screenHeight - position.getY(), position.getZ())) * Matrix4::scale(Vector3(size, size, 1));
	//modelMatrix = Matrix4::identity() * Matrix4::scale(Vector3(16,16,1));

	cellGcmSetBlendFunc(CELL_GCM_SRC_ALPHA, CELL_GCM_ONE_MINUS_SRC_ALPHA, CELL_GCM_SRC_ALPHA, CELL_GCM_ONE_MINUS_SRC_ALPHA);

	modelMatrix = Matrix4::translation(position) * Matrix4::scale(Vector3(32, 32, 1));
	textNode->SetTransform(modelMatrix);
	textNode->Update(0);
	viewMatrix = Matrix4::identity();
	projMatrix = Matrix4::orthographic(0.0f, (float)screenWidth, 0.0f, (float)screenHeight, -1.0f, 1.0f);
	textureMatrix = Matrix4::identity();
	currentVert->UpdateShaderMatrices(modelMatrix, viewMatrix, projMatrix);

	cellGcmSetBlendFunc(CELL_GCM_SRC_ALPHA, CELL_GCM_SRC_ALPHA, CELL_GCM_SRC_ALPHA, CELL_GCM_SRC_ALPHA);

	DrawNode(textNode);



	delete mesh; //Once it's drawn, we don't need it anymore!
}

void Renderer::displayInformation()
{
	cellGcmSetDepthTestEnable(CELL_GCM_FALSE);
	cellGcmSetDepthFunc(CELL_GCM_NEVER);

	this->SetCurrentShader(*currentVert, *currentFrag);

	//projMatrix = Matrix4::orthographic(-1, 1, 1, -1, -1, 1);
	projMatrix = Matrix4::orthographic(0.0f, (float)screenWidth, 0.0f, (float)screenHeight, -1.0f, 1.0f);
	textureMatrix = Matrix4::identity();
	viewMatrix = Matrix4::identity();
	currentVert->UpdateShaderMatrices(modelMatrix, viewMatrix, projMatrix);

	Matrix4 viewMatrixTemp = viewMatrix;
	Matrix4 projMatrixTemp = projMatrix;
	Matrix4 modelMatrixTemp = modelMatrix;
	int x = 64;

	/*DrawText("Physics FPS: " + to_string(PhysicsSystem::getFPS()), Vector3(0, 16, 0), 16, false);
	DrawText("Renderer FPS: " + to_string(fps), Vector3(0, 32, 0), 16, false);
	DrawText("Number of Nodes: " + to_string(nodeCount), Vector3(0, 48, 0), 16, false);*/

	//--------------------------------------------------GUI--------------------------------------------------------------------------------

	if(MyGame::GetGameClass().getCurrentState() == GAME_PLAYING)
	{
		std::ostringstream scoreStream; //output string stream
		scoreStream << score;
		std::string scoreString = scoreStream.str();

		DrawText("SCORE", Vector3(50, screenHeight - 30, 0), 25, false);
		DrawText(scoreString, Vector3(50, screenHeight - 70, 0), 25, false);


		std::ostringstream timeStream; //output string stream
		timeStream << timer;
		std::string timeString = timeStream.str();
		DrawText("Timer", Vector3(400, screenHeight - 30, 0), 25, false);
		DrawText(timeString, Vector3(400, screenHeight - 70, 0), 25, false);
	}
	else if (MyGame::GetGameClass().getCurrentState() == GAME_OVER)
	{
		std::ostringstream scoreStream; //output string stream
		scoreStream << score;
		std::string scoreString = scoreStream.str();


		DrawText("GAME OVER!!!",			Vector3(screenWidth/2 - 200,	screenHeight - screenHeight / 2 + 140,	0), 50,	false);
		DrawText("FINAL SCORE",				Vector3(screenWidth/2 - 180,	screenHeight - screenHeight / 2 + 70,	0),	50,	false);
		DrawText(scoreString,				Vector3(screenWidth/2 - 50,		screenHeight - screenHeight / 2,		0), 50,	false);
		DrawText("Thanks For Playing!!!",	Vector3(screenWidth/2 - 300,	screenHeight - screenHeight / 2 - 70,	0), 50,	false);
	}
	

	//--------------------------------------------------GUI--------------------------------------------------------------------------------

	viewMatrix = viewMatrixTemp;
	projMatrix = projMatrixTemp;
	modelMatrix = modelMatrixTemp;
}
