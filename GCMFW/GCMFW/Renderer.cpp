#include "Renderer.h"
#include "Input.h"

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


	pauseButtonIndex = 0;
	mainButtonIndex = 0;

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

	if (frameFrustum.InsideFrustum(*from))
	{
		if (from->GetColour().getW()< 1.0f)
		{
			transparentNodeList.push_back(from);
		}
		else
		{
			nodeList.push_back(from);
		}
	}

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


