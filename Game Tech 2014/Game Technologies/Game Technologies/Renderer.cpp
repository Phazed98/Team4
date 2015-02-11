#include "Renderer.h"

Renderer* Renderer::instance = NULL;

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	
{	
	camera			= NULL;

	root			= new SceneNode();

	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	simpleShader	= new Shader(SHADERDIR"TechVertex.glsl", SHADERDIR"TechFragment.glsl");
	textShader = new Shader(SHADERDIR"TexVertex.glsl", SHADERDIR"TexFragment.glsl");

	if (!simpleShader->LinkProgram() || !textShader->LinkProgram())
	{
		return;
	}

	instance		= this;

	init			= true;
}

Renderer::~Renderer(void)	
{
	delete root;
	delete simpleShader;
	delete textShader;

	currentShader = NULL;
}

void Renderer::UpdateScene(float msec)	
{

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_T)) //Build Cube
	{
		toggleWireFrame();
	}


	time += msec;

	if (time >= 1000)
	{
		fps = nbFrames;
		time = 0;
		nbFrames = 0;
	}

	nbFrames++;

	if(camera) 
	{
		camera->UpdateCamera(msec); 
	}
	root->Update(msec);
}

void Renderer::RenderScene()	
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	if(camera) 
	{
		SetCurrentShader(simpleShader);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

		textureMatrix.ToIdentity();
		modelMatrix.ToIdentity();
		viewMatrix		= camera->BuildViewMatrix();
		projMatrix		= Matrix4::Perspective(1.0f,10000.0f,(float)width / (float) height, 45.0f);
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

void	Renderer::DrawNode(SceneNode*n)	
{
	if(n->GetMesh()) 
	{
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"),	1,false, (float*)&(n->GetWorldTransform()*Matrix4::Scale(n->GetModelScale())));
		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"),1,(float*)&n->GetColour());

		n->Draw(*this);
	}
}

void	Renderer::BuildNodeLists(SceneNode* from)	
{
	Vector3 direction = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
	from->SetCameraDistance(Vector3::Dot(direction,direction));

	if(frameFrustum.InsideFrustum(*from)) 
	{
		if(from->GetColour().w < 1.0f) 
		{
			transparentNodeList.push_back(from);
		}
		else
		{
			nodeList.push_back(from);
		}
	}

	for(vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) 
	{
		BuildNodeLists((*i));
	}
}

void	Renderer::DrawNodes()	 
{
	for(vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); ++i ) {
		DrawNode((*i));
	}

	for(vector<SceneNode*>::const_reverse_iterator i = transparentNodeList.rbegin(); i != transparentNodeList.rend(); ++i ) 
	{
		DrawNode((*i));
	}
}

void	Renderer::SortNodeLists()	
{
	std::sort(transparentNodeList.begin(),	transparentNodeList.end(),	SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(),				nodeList.end(),				SceneNode::CompareByCameraDistance);
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
	SetCurrentShader(textShader);
	//Create a new temporary TextMesh, using our line of text andour font
	TextMesh* mesh = new TextMesh(text, *basicFont);

	Matrix4 viewMatrixTemp = viewMatrix;
	Matrix4 projMatrixTemp = projMatrix;
	//In ortho mode, we subtract the y from the height, so that a height of 0 is at the top left of the screen, which is more intuitive
	modelMatrix = Matrix4::Translation(Vector3(position.x, height - position.y, position.z)) * Matrix4::Scale(Vector3(size, size, 1));
	viewMatrix.ToIdentity();
	projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();

	mesh->Draw();

	delete mesh; //Once it's drawn, we don't need it anymore!

	viewMatrix = viewMatrixTemp;
	projMatrix = projMatrixTemp;
}

void Renderer::displayInformation()
{
	glDepthMask(false);
	glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
	DrawText("Physics FPS: " + to_string(PhysicsSystem::getFPS()), Vector3(0, 16, 0), 16, false);
	DrawText("Renderer FPS: " + to_string(fps), Vector3(0, 32, 0), 16, false);
	DrawText("Number of Nodes: " + to_string(nodeCount), Vector3(0, 48, 0), 16, false);

	DrawText("Controls:", Vector3(0, 112, 0), 16, false);
	DrawText("T = Toggle Wireframe", Vector3(0, 128, 0), 16, false);
	DrawText("WASD = Move", Vector3(0, 144, 0), 16, false);
	DrawText("Shift = Down", Vector3(0, 160, 0), 16, false);
	DrawText("Space = Up", Vector3(0, 176, 0), 16, false);
	//DrawText("(R)Reset Boxes", Vector3(0, 192, 0), 16, false);

	glDepthMask(true);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::toggleWireFrame()
{
	wireFrame = !wireFrame;

	if (wireFrame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}