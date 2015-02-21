#include "Renderer.h"

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

	std::cout << "New Root Has Been Made!!!!!" << std::endl;
	root = new SceneNode();
	
	instance = this;
	init = true;
}

Renderer::~Renderer(void)	
{
}

void Renderer::UpdateScene(float msec)
{
	//std::cout << "Updating Scene" << std::endl;

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

	cellGcmSetDepthTestEnable(CELL_GCM_TRUE);
	cellGcmSetDepthFunc(CELL_GCM_LESS);

	modelMatrix = Matrix4::identity();

	if (camera) 
	{
		viewMatrix = camera->BuildViewMatrix();
	}
	else
	{
		viewMatrix = Matrix4::identity();
	}

	currentVert->UpdateShaderMatrices(modelMatrix, viewMatrix, projMatrix);

	BuildNodeLists(root);
	SortNodeLists();
	DrawNodes();
	ClearNodeLists();

	SwapBuffers();
}

void	Renderer::SetCamera(Camera*c)
{
	camera = c;
}

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


