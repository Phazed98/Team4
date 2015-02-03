#include "MyGame.h"
#include "InertialMatrixHelper.h"
#include "Spring.h"
#include "SpringDemo.h"

/*
Creates a really simple scene for our game - A cube robot standing on
a floor. As the module progresses you'll see how to get the robot moving
around in a physically accurate manner, and how to stop it falling
through the floor as gravity is added to the scene. 

You can completely change all of this if you want, it's your game!

*/
MyGame::MyGame()	{
	gameCamera = new Camera(-30.0f,0.0f,Vector3(0,350,-800)); //changed the location Daixi 3.2.2015

	Renderer::GetRenderer().SetCamera(gameCamera);

	CubeRobot::CreateCube();

	PlayerPosition=Vector3(0,100,-400);//new 2.2.2015 Daixi

	/*
	We're going to manage the meshes we need in our game in the game class!

	You can do this with textures, too if you want - but you might want to make
	some sort of 'get / load texture' part of the Renderer or OGLRenderer, so as
	to encapsulate the API-specific parts of texture loading behind a class so
	we don't care whether the renderer is OpenGL / Direct3D / using SOIL or 
	something else...
	*/
	cube	= new OBJMesh(MESHDIR"cube.obj");
	quad	= Mesh::GenerateQuad();
	sphere	= new OBJMesh(MESHDIR"ico.obj");
	PlayerMesh = new OBJMesh(MESHDIR"Player.obj"); // 3.2.2015 Daixi

	/*
	A more 'robust' system would check the entities vector for duplicates so as
	to not cause problems...why not give it a go?
	*/
	GameEntity* quadEntity = BuildQuadEntity(1000.0f);
	allEntities.push_back(quadEntity);
	allEntities.push_back(BuildRobotEntity());
	
	GameEntity* ball0 = BuildSphereEntity(100.0f, Vector3(-300, 300, -100), Vector3(0, 0, 0));
	allEntities.push_back(ball0);
	GameEntity* ball1 = BuildSphereEntity(100.0f, Vector3(-100, 300, -100), Vector3(0, 0, 0));
	allEntities.push_back(ball1);

	GameEntity* charactor = BuildPlayerEntity(20.0f,Vector3(0,0,0));   //new 2.2.2015  Daixi
	Player = charactor;   


	Spring* s = new Spring(&ball0->GetPhysicsNode(), Vector3(0,100,0), &ball1->GetPhysicsNode(), Vector3(0,-100,0));
	
	PhysicsSystem::GetPhysicsSystem().AddConstraint(s);
	PhysicsSystem::GetPhysicsSystem().AddDebugDraw(s);


	s = new Spring(&ball1->GetPhysicsNode(), Vector3(0,100,0), &quadEntity->GetPhysicsNode(), Vector3(0,-100,-600)); // Note that these are relative positions and the quad is already rotated
	
	PhysicsSystem::GetPhysicsSystem().AddConstraint(s);
	PhysicsSystem::GetPhysicsSystem().AddDebugDraw(s);

	SpringDemo* demo = new SpringDemo();
	
	PhysicsSystem::GetPhysicsSystem().AddConstraint(demo);
	PhysicsSystem::GetPhysicsSystem().AddDebugDraw(demo);
	
}

MyGame::~MyGame(void)	
{
	/*
	We're done with our assets now, so we can delete them
	*/
	delete cube;
	delete quad;
	delete sphere;

	CubeRobot::DeleteCube();

	//GameClass destructor will destroy your entities for you...
}

/*
Here's the base 'skeleton' of your game update loop! You will presumably
want your games to have some sort of internal logic to them, and this
logic will be added to this function.
*/

//new 3.2.2015    Daixi

void MyGame::UpdatePlayer(float msec){
	PlayerPosition = Player->GetPhysicsNode().GetPosition();

	if(Player->GetPhysicsNode().GetOrientation().z<-0.3||Player->GetPhysicsNode().GetOrientation().z>0.3){
		Player->GetPhysicsNode().SetAngularVelocity(Vector3(0,0,0));
	}

	if(Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
		PlayerPosition=PlayerPosition + Vector3(1,0,0)*msec/10;
		if(Player->GetPhysicsNode().GetOrientation().z>=-0.3){
			Player->GetPhysicsNode().SetAngularVelocity(Vector3(0,0,-0.0002));
		}
		if(Player->GetPhysicsNode().GetOrientation().z>0){
			Player->GetPhysicsNode().SetAngularVelocity(Vector3(0,0,-0.0007));
		}
	}

	if(Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
		PlayerPosition=PlayerPosition + Vector3(-1,0,0)*msec/10;
		if(Player->GetPhysicsNode().GetOrientation().z<=0.3){
			Player->GetPhysicsNode().SetAngularVelocity(Vector3(0,0,0.0002));
		}
		if(Player->GetPhysicsNode().GetOrientation().z<0){
			Player->GetPhysicsNode().SetAngularVelocity(Vector3(0,0,0.0007));
		}
	}

	if(!Window::GetKeyboard()->KeyDown(KEYBOARD_D)&&!Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
		if(Player->GetPhysicsNode().GetOrientation().z==0){
		Player->GetPhysicsNode().SetAngularVelocity(Vector3(0,0,0));
		}
		if(Player->GetPhysicsNode().GetOrientation().z<0){
			Player->GetPhysicsNode().SetAngularVelocity(Vector3(0,0,0.0004));
		}
		if(Player->GetPhysicsNode().GetOrientation().z>0){
			Player->GetPhysicsNode().SetAngularVelocity(Vector3(0,0,-0.0004));
		}
	}

	if(Player->GetPhysicsNode().GetPosition().y<=100){
		Player->GetPhysicsNode().SetLinearVelocity(Vector3(0,0,0));
		Player->GetPhysicsNode().SetUseGravity(FALSE);
	}
	if(Player->GetPhysicsNode().GetLinearVelocity()==Vector3(0,0,0)){
		if(Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)){
			Player->GetPhysicsNode().SetInverseMass(10);
			Player->GetPhysicsNode().SetUseGravity(TRUE);
			Player->GetPhysicsNode().SetLinearVelocity(Vector3(0,0.6,0));	
		}
	}
	Player->GetPhysicsNode().SetPosition(PlayerPosition);
}



void MyGame::UpdateGame(float msec) {
	if(gameCamera) {
		gameCamera->UpdateCamera(msec);
		UpdatePlayer(msec);//new 3.2.2015 Daixi
	}

	for(vector<GameEntity*>::iterator i = allEntities.begin(); i != allEntities.end(); ++i) {
		(*i)->Update(msec);
	}

	/*
	Here's how we can use OGLRenderer's inbuilt debug-drawing functions! 
	I meant to talk about these in the graphics module - Oops!

	We can draw squares, lines, crosses and circles, of varying size and
	colour - in either perspective or orthographic mode.

	Orthographic debug drawing uses a 'virtual canvas' of 720 * 480 - 
	that is 0,0 is the top left, and 720,480 is the bottom right. A function
	inside OGLRenderer is provided to convert clip space coordinates into
	this canvas space coordinates. How you determine clip space is up to you -
	maybe your renderer has getters for the view and projection matrix?

	Or maybe your Camera class could be extended to contain a projection matrix?
	Then your game would be able to determine clip space coordinates for its
	active Camera without having to involve the Renderer at all?

	Perspective debug drawing relies on the view and projection matrices inside
	the renderer being correct at the point where 'SwapBuffers' is called. As
	long as these are valid, your perspective drawing will appear in the world.

	This gets a bit more tricky with advanced rendering techniques like deferred
	rendering, as there's no guarantee of the state of the depth buffer, or that
	the perspective matrix isn't orthographic. Therefore, you might want to draw
	your debug lines before the inbuilt position before SwapBuffers - there are
	two OGLRenderer functions DrawDebugPerspective and DrawDebugOrtho that can
	be called at the appropriate place in the pipeline. Both take in a viewProj
	matrix as an optional parameter.

	Debug rendering uses its own debug shader, and so should be unaffected by
	and shader changes made 'outside' of debug drawing


	Lets draw a box around the cube robot!
	*/

	Renderer::GetRenderer().DrawDebugBox(DEBUGDRAW_PERSPECTIVE, Vector3(0,51,0), Vector3(100,100,100), Vector3(1,0,0));

	////We'll assume he's aiming at something...so let's draw a line from the cube robot to the target
	////The 1 on the y axis is simply to prevent z-fighting!
	Renderer::GetRenderer().DrawDebugLine(DEBUGDRAW_PERSPECTIVE, Vector3(0,1,0),Vector3(200,1,200), Vector3(0,0,1), Vector3(1,0,0));

	////Maybe he's looking for treasure? X marks the spot!
	Renderer::GetRenderer().DrawDebugCross(DEBUGDRAW_PERSPECTIVE, Vector3(200,1,200),Vector3(50,50,50), Vector3(0,0,0));

	////CubeRobot is looking at his treasure map upside down!, the treasure's really here...
	Renderer::GetRenderer().DrawDebugCircle(DEBUGDRAW_PERSPECTIVE, Vector3(-200,1,-200), 50.0f, Vector3(0,1,0));

	PhysicsSystem::GetPhysicsSystem().DrawDebug();
}

/*
Makes an entity that looks like a CubeRobot! You'll probably want to modify
this so that you can have different sized robots, with different masses and
so on!
*/
GameEntity* MyGame::BuildRobotEntity() {
	GameEntity*g = new GameEntity(new CubeRobot(), new PhysicsNode());

	g->GetPhysicsNode().SetUseGravity(false);
	g->ConnectToSystems();
	return g;
}

/*
Makes a cube. Every game has a crate in it somewhere!
*/
GameEntity* MyGame::BuildCubeEntity(float size) {
	GameEntity*g = new GameEntity(new SceneNode(cube), new PhysicsNode());
	g->ConnectToSystems();

	SceneNode &test = g->GetRenderNode();

	test.SetModelScale(Vector3(size,size,size));
	test.SetBoundingRadius(size);

	return g;
}

/*
Makes a Player. Every game has a crate in it somewhere!
*/
GameEntity* MyGame::BuildPlayerEntity(float size, Vector3 pos) {   //new 2.2.2015 Daixi
	SceneNode* test = new SceneNode(PlayerMesh);
	test->SetModelScale(Vector3(size,size,size));
	test->SetBoundingRadius(size);
	test->SetColour(Vector4(0.2,0.2,0.5,1));
	PhysicsNode*p = new PhysicsNode();

	p->SetUseGravity(false);
	pos = Vector3(0,100,-400);
	p->SetPosition(pos);


	GameEntity*g = new GameEntity(test, p);
	g->ConnectToSystems();
	return g;
}

/*
Makes a sphere.
*/
GameEntity* MyGame::BuildSphereEntity(float radius, Vector3 pos, Vector3 vel) {
	SceneNode* s = new SceneNode(sphere);

	s->SetModelScale(Vector3(radius,radius,radius));
	s->SetBoundingRadius(radius);
	s->SetColour(Vector4(0.0,1.0,0.5,1));
	PhysicsNode*p = new PhysicsNode();
	p->SetPosition(pos);
	p->SetLinearVelocity(vel);
	p->SetAngularVelocity(Vector3(0,0,0.01f));
	p->SetInverseInertia(InertialMatrixHelper::createSphereInvInertial(1.0f, radius));
	p->SetInverseMass(1.0f);
	p->SetCollisionVolume(new CollisionSphere(radius));
	GameEntity*g = new GameEntity(s, p);
	g->ConnectToSystems();
	return g;
}

/*
Makes a flat quad, initially oriented such that we can use it as a simple
floor. 
*/
GameEntity* MyGame::BuildQuadEntity(float size) {
	SceneNode* s = new SceneNode(quad);

	s->SetModelScale(Vector3(size,size,size));
	//Oh if only we had a set texture function...we could make our brick floor again WINK WINK
	s->SetBoundingRadius(size);

	PhysicsNode*p = new PhysicsNode(Quaternion::AxisAngleToQuaterion(Vector3(1,0,0), 90.0f), Vector3());
	p->SetUseGravity(false);
	p->SetInverseMass(0.0f);
	p->SetInverseInertia(InertialMatrixHelper::createImmovableInvInertial());
	p->SetCollisionVolume(new CollisionPlane(Vector3(0,1,0), 0));
	GameEntity*g = new GameEntity(s, p);
	g->ConnectToSystems();
	return g;
}