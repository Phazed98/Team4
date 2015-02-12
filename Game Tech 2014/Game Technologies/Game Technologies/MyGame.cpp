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
MyGame::MyGame()	
{

	count_time = 0;    //new control shoot the bullets   4.2.2015 Daixi

	Speed_Player = 1;  //control the player speed

	elements.push_back(top);
	elements.push_back(right);
	elements.push_back(bottom);
	elements.push_back(left);

	reference.push_back(top);
	reference.push_back(right);
	reference.push_back(bottom);
	reference.push_back(left);

	obstacleElements.resize(4);
	obstacleReference.resize(4);

	for (int i = 0; i < 4; i++)
	{
		obstacleReference[i] = NULL;
	}

	gameCamera = new Camera(-30.0f, 0.0f, Vector3(0, 350, -800)); //changed the location Daixi 3.2.2015

	Renderer::GetRenderer().SetCamera(gameCamera);

	CubeRobot::CreateCube();

	Car = new Vehicle();
	PlayerMesh = new OBJMesh(MESHDIR"Player.obj"); // 3.2.2015 Daixi


	cube	= new OBJMesh(MESHDIR"cube.obj");
	quad	= Mesh::GenerateQuad();
	sphere	= new OBJMesh(MESHDIR"ico.obj");
	PlayerMesh = new OBJMesh(MESHDIR"Player.obj"); // 3.2.2015 Daixi

	Enemy = BuildPlayerEntity(20.0f, Vector3(-300, 100, -300)); //new 4.2.2015 Daixi
	Enemy->GetPhysicsNode().SetPosition(Vector3(300, 100, -100));

	Position0 = Enemy->GetPhysicsNode().GetPosition(); //5.2.2015 Daixi ------------------This is the straight line bullet
	Position0.z = Position0.z - 20;
	bullet = new Bullets(Position0);

	BuffEntity = BuildBuffEntity(6, Vector3(0, 100, -200)); //6.2.2015 Daixi ------------------ This is the buff object, and when player hit it, will speed up
	BuffEntity->GetRenderNode().SetColour(Vector4(1, 1, 0, 1));

	/*
	A more 'robust' system would check the entities vector for duplicates so as
	to not cause problems...why not give it a go?
	*/
	//GameEntity* quadEntity = BuildQuadEntity(1000.0f);
	//allEntities.push_back(quadEntity);
	//allEntities.push_back(BuildRobotEntity());
	//
	//GameEntity* ball0 = BuildSphereEntity(100.0f, Vector3(-300, 300, -100), Vector3(0, 0, 0));
	//allEntities.push_back(ball0);
	//GameEntity* ball1 = BuildSphereEntity(100.0f, Vector3(-100, 300, -100), Vector3(0, 0, 0));
	//allEntities.push_back(ball1);

	//Spring* s = new Spring(&ball0->GetPhysicsNode(), Vector3(0,100,0), &ball1->GetPhysicsNode(), Vector3(0,-100,0));
	//
	//PhysicsSystem::GetPhysicsSystem().AddConstraint(s);
	//PhysicsSystem::GetPhysicsSystem().AddDebugDraw(s);

	//// Note that these are relative positions and the quad is already rotated
	//s = new Spring(&ball1->GetPhysicsNode(), Vector3(0,100,0), &quadEntity->GetPhysicsNode(), Vector3(0,-100,-600)); 
	//
	//PhysicsSystem::GetPhysicsSystem().AddConstraint(s);
	//PhysicsSystem::GetPhysicsSystem().AddDebugDraw(s);

	//SpringDemo* demo = new SpringDemo();
	//
	//PhysicsSystem::GetPhysicsSystem().AddConstraint(demo);
	//PhysicsSystem::GetPhysicsSystem().AddDebugDraw(demo);

	//-------------------------------------------------Planes---------------------------------------------------------//

	GameEntity* plane = BuildObjectEntity(200, 0, 0);
	allEntities.push_back(plane);

	plane = BuildObjectEntity(200, 0, 1);
	allEntities.push_back(plane);

	plane = BuildObjectEntity(200, 0, 2);
	allEntities.push_back(plane);

	plane = BuildObjectEntity(200, 0, 3);
	allEntities.push_back(plane);

}

MyGame::~MyGame(void)	
{
	/*
	We're done with our assets now, so we can delete them
	*/
	delete cube;
	delete quad;
	delete sphere;
	delete Enemy;   //new 5.2.2015 Daixi
	delete Car;
	delete bullet;

	CubeRobot::DeleteCube();

	elements.clear();
	reference.clear();
	obstacleElements.clear();
	obstacleReference.clear();

	//GameClass destructor will destroy your entities for you...
}


void MyGame::UpdateGame(float msec) 
{
	if(gameCamera) 
	{
		Car->UpdatePlayer(msec);
		gameCamera->SetPosition(Car->tempPosition);
		gameCamera->UpdateCamera(msec);
	}

	for(vector<GameEntity*>::iterator i = allEntities.begin(); i != allEntities.end(); ++i) 
	{
		(*i)->Update(msec);
	
	}

	if (count_time == 80)
	{    //new control when shoot the bullets   4.2.2015 Daixi
		bullet->ShootBullets();
		count_time = 0;
	}

	count_time++;

	handlePlanes();

	//Renderer::GetRenderer().DrawDebugBox(DEBUGDRAW_PERSPECTIVE, Vector3(0,51,0), Vector3(100,100,100), Vector3(1,0,0));

	//////We'll assume he's aiming at something...so let's draw a line from the cube robot to the target
	//////The 1 on the y axis is simply to prevent z-fighting!
	//Renderer::GetRenderer().DrawDebugLine(DEBUGDRAW_PERSPECTIVE, Vector3(0,1,0),Vector3(200,1,200), Vector3(0,0,1), Vector3(1,0,0));

	//////Maybe he's looking for treasure? X marks the spot!
	//Renderer::GetRenderer().DrawDebugCross(DEBUGDRAW_PERSPECTIVE, Vector3(200,1,200),Vector3(50,50,50), Vector3(0,0,0));

	//////CubeRobot is looking at his treasure map upside down!, the treasure's really here...
	//Renderer::GetRenderer().DrawDebugCircle(DEBUGDRAW_PERSPECTIVE, Vector3(-200,1,-200), 50.0f, Vector3(0,1,0));

	//PhysicsSystem::GetPhysicsSystem().DrawDebug();
}

GameEntity* MyGame::BuildPlayerEntity(float size, Vector3 pos)
{
	SceneNode* test = new SceneNode(PlayerMesh);
	test->SetModelScale(Vector3(size, size, size));
	test->SetBoundingRadius(size);
	test->SetColour(Vector4(0.2, 0.2, 0.5, 1));
	PhysicsNode*p = new PhysicsNode();

	p->SetUseGravity(false);
	pos = Vector3(0, 100, -400);
	p->SetPosition(pos);
	p->SetCollisionVolume(new CollisionSphere(size));


	GameEntity*g = new GameEntity(test, p);
	g->ConnectToSystems();
	return g;
}

/*
Makes Buff. it can speed up Player or slow down!
*/
GameEntity* MyGame::BuildBuffEntity(float radius, Vector3 pos)
{   //new 6.2.2015 Daixi
	SceneNode* test = new SceneNode(sphere);
	test->SetModelScale(Vector3(radius, radius, radius));
	test->SetBoundingRadius(radius);
	test->SetColour(Vector4(0.2, 0.2, 0.5, 1));
	PhysicsNode*p = new PhysicsNode();

	p->SetUseGravity(false);
	p->SetPosition(pos);

	GameEntity*g = new GameEntity(test, p);
	g->ConnectToSystems();
	return g;
}

/*
Makes an entity that looks like a CubeRobot! You'll probably want to modify
this so that you can have different sized robots, with different masses and
so on!
*/
GameEntity* MyGame::BuildRobotEntity() 
{
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
	

	SceneNode &test = g->GetRenderNode();

	test.SetModelScale(Vector3(size,size,size));
	test.SetBoundingRadius(size);

	g->ConnectToSystems();
	return g;
}
/*
Makes a sphere.
*/
GameEntity* MyGame::BuildSphereEntity(float radius, Vector3 pos, Vector3 vel) 
{
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
GameEntity* MyGame::BuildQuadEntity(float size) 
{
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

ObjectType* MyGame::BuildObjectEntity(float size, int type, int subType) {
	SceneNode* s = new SceneNode(cube);
	PhysicsNode* p = new PhysicsNode();
	p->SetUseGravity(false);

	ObjectType*g = new ObjectType(s, p, type, subType);
	g->ConnectToSystems();
	SceneNode &test = g->GetRenderNode();

	test.SetModelScale(Vector3(size, size / 2, size * 2));
	test.SetBoundingRadius(size);

	//50% chance that we set the random value of this plane to a random value from 1 - 5
	if ((rand() % 100 + 1) > 50)
	{
		if (getDrawingPlanes(subType) > 1)
		{
			g->setRandom(rand() % 5 + 1);
		}
	}

	elements[subType].push_back(g);

	/*if reference vector of this subtype has objects inside delete them all
	  so it contains only the one that was created last */
	while(reference[subType].size() > 0)
	{
		reference[subType].pop_back();
	}
	reference[subType].push_back(g);
	
	return g;
}

//creates a new Obstacle
Obstacle* MyGame::BuildObstacleEntity(float size, int type, int subType, ObjectType* _obj) {
	SceneNode* s = new SceneNode(sphere);
	PhysicsNode* p = new PhysicsNode();
	p->SetUseGravity(false);

	Obstacle*g = new Obstacle(_obj, s, p, type, subType, 0);
	g->ConnectToSystems();
	SceneNode &test = g->GetRenderNode();

	test.SetModelScale(Vector3(size, size, size));
	test.SetBoundingRadius(size);

	obstacleElements[subType].push_back(g);

	return g;
}

int MyGame::getIndexOfAllEtities(GameEntity* _G)
{
	int index = -1;
	for (int i = 0; i < allEntities.size(); i++)
	{
		if (_G == allEntities[i])
		{
			index = i;
			break;
		}
	}

	return index;
}

int MyGame::getIndexOfElements(ObjectType* _G)
{
	for (int j = 0; j < elements[_G->getSubType()].size(); j++)
	{
		if (_G == elements[_G->getSubType()][j])
		{
			return j;
		}
	}
	return 0;
}

/* We get the number of planes that are not generating gaps infront of them 
   which should always be at least one*/
int MyGame::getDrawingPlanes(int _subType)
{
	int count = 0;
	for (int i = 0; i <4; i++)
	{
		if (reference[i].size() > 0)
		{
			if (reference[i][0]->getRandom() == 1 && i != _subType)
			{
				count++;
			}
		}
	}
	return count;
}

int MyGame::getEmptyIndex(int _subType)
{
	for (int i = 0; i < elements[_subType].size(); i++)
	{
		if (elements[_subType][i]->getState() == 4)
		{
			return i;
		}
	}
	return -1;
}

void MyGame::handlePlanes()
{
	for (int i = 0; i < 4; i++)
	{
		// iterate through all obstacles
		for (int j = 0; j < obstacleElements[i].size(); j++)
		{
			//check if the Obstacel has reached at the end
			if (obstacleElements[i][j]->getState() == 1)
			{
				//set the new state so that it can be re used
				obstacleElements[i][j]->setState(2);

				//remove from all Entities so that we dont waste the update calls when not moving.
				allEntities.erase(allEntities.begin() + getIndexOfAllEtities(obstacleElements[i][j]));
			}
		}
		// iterate through all Tiles
		for (int j = 0; j < elements[i].size(); j++)
		{
			if (elements[i][j]->getState() == 1)
			{

				//create new
				if (getEmptyIndex(i) == -1)
				{
					allEntities.push_back(BuildObjectEntity(200, 0, i));
				}
				else
				{
					int x = getEmptyIndex(i);

					elements[i][x]->reset();
					while (reference[i].size()>0)
						reference[i].pop_back();

					reference[i].push_back(elements[i][x]);
					if ((rand() % 100 + 1) > 50)
					{
						if (getDrawingPlanes(i) > 0)
						{
							elements[i][x]->setRandom(rand() % 5 + 1);
						}
					}
					allEntities.push_back(elements[i][x]);

					//create obstacle
					if ((rand() % 100 + 1) > 50)
						CreateObstacle(elements[i][x]);
				}
				//change state
				elements[i][j]->setState(2);

			}
			else if (elements[i][j]->getState() == 3)
			{

				elements[i][j]->setState(4);

				allEntities.erase(allEntities.begin() + getIndexOfAllEtities(elements[i][j]));
				

			}
		}
	}
}

void MyGame::CreateObstacle(ObjectType* _obj)
{
	int random_number = rand() % 2 + 1;
	Obstacle* temp = NULL;
	int empty = getObstacleEmptyIndex(_obj->getSubType(),0);
	// for the first obstacle created
	if (obstacleReference[_obj->getSubType()] == NULL)
	{
		temp = BuildObstacleEntity(50, 1, _obj->getSubType(), _obj);
		obstacleElements[_obj->getSubType()].push_back(temp);
	}
	//reference exists, but everything is running/working
	else if (empty == -1)
	{
		if (obstacleReference[_obj->getSubType()] != NULL)
		{
			if (obstacleReference[_obj->getSubType()]->GetPhysicsNode().GetPosition().z < -800.0f)
			{
				temp = BuildObstacleEntity(50, 1, _obj->getSubType(), _obj);
				obstacleElements[_obj->getSubType()].push_back(temp);
			}
		}
	}
	//free obstacle exists 
	else 
	{
		//use the old object
		if (obstacleReference[_obj->getSubType()] != NULL)
		{
			if (obstacleReference[_obj->getSubType()]->GetPhysicsNode().GetPosition().z < -800.0f)
			{
				temp = obstacleElements[_obj->getSubType()][empty];
				temp->SetTile(_obj);
			}
		}
	}
	//initialize values and push to allEntities
	if (temp != NULL)
	{
		int temp_lane = (_obj->getSubType() + 1) * 2 - random_number;
		temp->SetLane(temp_lane);
		temp->resetObstacle();
		obstacleReference[_obj->getSubType()] = temp;
		allEntities.push_back(temp);
	}
}



//returns the index of the Obstacle which is free , if not , returns -1
int MyGame::getObstacleEmptyIndex(int _subType, int _obstacleType)
{
	for (int i = 0; i < obstacleElements[_subType].size(); i++)
	{
		if (obstacleElements[_subType][i]!=NULL)
		if (obstacleElements[_subType][i]->getState() == 2 && _obstacleType == obstacleElements[_subType][i]->getObstacleType())
		{
			return i;
		}
	}
	return -1;
}