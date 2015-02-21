#include "MyGame.h"
#include "InertialMatrixHelper.h"
//#include "Spring.h"
//#include "SpringDemo.h"

/*
Creates a really simple scene for our game - A cube robot standing on
a floor. As the module progresses you'll see how to get the robot moving
around in a physically accurate manner, and how to stop it falling
through the floor as gravity is added to the scene.

You can completely change all of this if you want, it's your game!

*/
MyGame::MyGame()
{
	std::cout << "Loadede MyGame Constructor and Mesh" << std::endl;
	timer = 0;
	gameCamera = new Camera(); //changed the location Daixi 3.2.2015
	Renderer::GetRenderer().SetCamera(gameCamera);

	cube = new OBJMesh(MESHDIR"cube.obj");
	cube->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/borg.gtf"));


	sphere = new OBJMesh(MESHDIR"SR-71_Blackbird.obj");
	sphere->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/borg.gtf"));

	quad = Mesh::GenerateQuad();

	GameEntity* cubeEntity = BuildCubeEntity(50);
	allEntities.push_back(cubeEntity);

	GameEntity* sphereEntity = BuildCubeEntity(80,Vector3(20,10,30));
	allEntities.push_back(sphereEntity);
}

MyGame::~MyGame(void)
{
	/*
	We're done with our assets now, so we can delete them
	*/
	delete cube;
	delete quad;
	delete sphere;
	//GameClass destructor will destroy your entities for you...
}

/*
Here's the base 'skeleton' of your game update loop! You will presumably
want your games to have some sort of internal logic to them, and this
logic will be added to this function.
*/
void MyGame::UpdateGame(float msec)
{
	if (gameCamera)
	{
		gameCamera->Update(msec);
	}

	for (std::vector<GameEntity*>::iterator i = allEntities.begin(); i != allEntities.end(); ++i)
	{
		(*i)->Update(msec);
	}
}

/*
Makes an entity that looks like a CubeRobot! You'll probably want to modify
this so that you can have different sized robots, with different masses and
so on!
*/
GameEntity* MyGame::BuildRobotEntity()
{
	//GameEntity*g = new GameEntity(new CubeRobot(), new PhysicsNode());

	//g->GetPhysicsNode().SetUseGravity(false);
	//g->ConnectToSystems();
	//return g;
}

/*
Makes a cube. Every game has a crate in it somewhere!
*/
GameEntity* MyGame::BuildCubeEntity(float size, Vector3 pos) 
{
	SceneNode* logo = new SceneNode();
	logo->SetMesh(sphere); //tell it to draw our new quad
	logo->SetTransform(Matrix4::scale(Vector3(size, size, size))* Matrix4::translation(pos));
	GameEntity*g = new GameEntity(logo, new PhysicsNode());
	g->ConnectToSystems();

	return g;
}

/*
Makes a sphere.
*/
GameEntity* MyGame::BuildSphereEntity(float radius, Vector3 pos, Vector3 vel)
{
	SceneNode* s = new SceneNode(sphere);
	s->SetModelScale(Vector3(radius, radius, radius));
	s->SetBoundingRadius(radius);
	s->SetColour(Vector4(0.0, 1.0, 0.5, 1));
	
	PhysicsNode*p = new PhysicsNode();
	p->SetPosition(pos);
	p->SetLinearVelocity(vel);
	p->SetAngularVelocity(Vector3(0, 0, 0.01f));
	p->SetInverseInertia(InertialMatrixHelper::createSphereInvInertial(1.0f, radius));
	p->SetInverseMass(1.0f);
	p->SetCollisionVolume(new CollisionSphere(radius));
	GameEntity*g = new GameEntity(s, p);
	g->ConnectToSystems();
	return g;
}

/*
Makes a flat quad
*/
GameEntity* MyGame::BuildQuadEntity(float size)
{
	SceneNode* s = new SceneNode(quad);

	s->SetModelScale(Vector3(size, size, size));
	//Oh if only we had a set texture function...we could make our brick floor again WINK WINK
	s->SetBoundingRadius(size);

	PhysicsNode*p = new PhysicsNode();
	p->SetUseGravity(false);
	p->SetInverseMass(0.0f);
	p->SetInverseInertia(InertialMatrixHelper::createImmovableInvInertial());
	p->SetCollisionVolume(new CollisionPlane(Vector3(0, 1, 0), 0));
	GameEntity*g = new GameEntity(s, p);
	g->ConnectToSystems();
	return g;
}