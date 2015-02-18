#include "Renderer.h"

Renderer* Renderer::instance = NULL;

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{	
	camera			= NULL;

	root			= new SceneNode();
	bbScene = new SceneNode();

	quad = Mesh::GenerateQuad();
	quad->SetTexture(SOIL_load_OGL_texture( TEXTUREDIR"bbgrass.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	

	
	simpleShader = new Shader(SHADERDIR"TechVertex.glsl", SHADERDIR"TechFragment.glsl");
	billboardShader = new Shader(SHADERDIR"billboardVS.glsl", SHADERDIR"billboardFS.glsl", SHADERDIR"billboardGS.glsl");
	if(!simpleShader->LinkProgram()||!billboardShader->LinkProgram() ){
		return;
	}
	
	
	SceneNode* testQuad = new SceneNode();
	testQuad->SetTransform(Matrix4::Translation(Vector3(0, 400, 0)));
	testQuad->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
	testQuad->SetBoundingRadius(1000.0f);
	testQuad->SetMesh(quad);

	//bbScene->AddChild(testQuad);
	root->AddChild(testQuad);

	fireParticleSystem.InitParticleSystem(0, Vector3(0, 500, 0));//NEW!!

	earthParticleSystem.InitParticleSystem(0, Vector3(100, 500, 0));

	instance		= this;

	init			= true;
}

Renderer::~Renderer(void)	{
	delete root;
	delete simpleShader;

	currentShader = NULL;
}

void Renderer::UpdateScene(float msec)	{
	if(camera) {
		camera->UpdateCamera(msec); 
	}
	
	this->msec = msec;
	root->Update(msec);
}

void Renderer::RenderScene()	{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	if(camera) {
		

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
		
		DrawQuads();
		/*SetCurrentShader(billboardShader);

		BuildNodeLists(bbScene);
		SortNodeLists();
		DrawNodes();
		ClearNodeLists();*/

		

	}

	glUseProgram(0);

	fireParticleSystem.Render(msec, viewMatrix, projMatrix);//NEW!!

	earthParticleSystem.Render(msec, viewMatrix, projMatrix);

	
	
	SwapBuffers();
}

void	Renderer::DrawQuads(){
	
	SetCurrentShader(billboardShader);

	
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), 1);
	glUniform4f(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"), 1, 1, 1, 1);

	BuildNodeLists(bbScene);
	SortNodeLists();
	DrawNodes();
	ClearNodeLists();

}

void	Renderer::DrawNode(SceneNode*n)	{
	if(n->GetMesh()) {
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"),	1,false, (float*)&(n->GetWorldTransform()*Matrix4::Scale(n->GetModelScale())));
		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"),1,(float*)&n->GetColour());

		n->Draw(*this);
	}
}

void	Renderer::BuildNodeLists(SceneNode* from)	{
	Vector3 direction = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
	from->SetCameraDistance(Vector3::Dot(direction,direction));

	if(frameFrustum.InsideFrustum(*from)) {
		if(from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else{
			nodeList.push_back(from);
		}
	}

	for(vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));
	}
}

void	Renderer::DrawNodes()	 {
	for(vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); ++i ) {
		DrawNode((*i));
	}

	for(vector<SceneNode*>::const_reverse_iterator i = transparentNodeList.rbegin(); i != transparentNodeList.rend(); ++i ) {
		DrawNode((*i));
	}
}

void	Renderer::SortNodeLists()	{
	std::sort(transparentNodeList.begin(),	transparentNodeList.end(),	SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(),				nodeList.end(),				SceneNode::CompareByCameraDistance);
}

void	Renderer::ClearNodeLists()	{
	transparentNodeList.clear();
	nodeList.clear();
}

void	Renderer::SetCamera(Camera*c) {
	camera = c;
}

void	Renderer::AddNode(SceneNode* n) {
	root->AddChild(n);
}

void	Renderer::RemoveNode(SceneNode* n) {
	root->RemoveChild(n);
}