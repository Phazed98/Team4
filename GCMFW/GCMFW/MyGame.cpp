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
	//std::cout << "Loadede MyGame Constructor and Mesh" << std::endl;
	//timer = 0;
	

	//cube = new OBJMesh(MESHDIR"cube.obj");
	//cube->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/borg.gtf"));

	////sphere = new OBJMesh(MESHDIR"SR-71_Blackbird.obj");
	//sphere = new OBJMesh(MESHDIR"cube.obj");
	//sphere->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/borg.gtf"));

	//quad = Mesh::GenerateQuad();

	std::cout << "Loaded MyGame Constructor and Mesh" << std::endl;
	//Set Timers and Speed
	timer = 0;
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


	cube = new OBJMesh(MESHDIR"cube.obj");
	cube->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/borg.gtf"));
	cubeAir = new OBJMesh(MESHDIR"cube.obj");
	cubeAir->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/borg.gtf"));
	cubeWater = new OBJMesh(MESHDIR"cube.obj");
	cubeWater->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/borg.gtf"));
	cubeFire = new OBJMesh(MESHDIR"cube.obj");
	cubeFire->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/borg.gtf"));
	cubeEarth = new OBJMesh(MESHDIR"cube.obj");
	cubeEarth->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/borg.gtf"));
	quad = Mesh::GenerateQuad();

	gameCamera = new Camera(); //changed the location Daixi 3.2.2015
	Renderer::GetRenderer().SetCamera(gameCamera);


	//-------------------------------------------------Planes---------------------------------------------------------//

	for (int i = 0; i < 7; i++)
	{
		ObjectType* plane = BuildObjectEntity(0, 2);
		plane->SetPos(Vector3(0.0f, -500.0f, -i * 800.0f));
		
		if (i != 6)
			plane->setState(2);
		
		allEntities.push_back(plane);
	}

	for (int i = 0; i < 7; i++)
	{
		ObjectType* plane = BuildObjectEntity(0, 0);
		plane->SetPos(Vector3(0.0f, 500.0f, -i * 800.0f));
		
		if (i != 6)
			plane->setState(2);
		
		allEntities.push_back(plane);
	}

	for (int i = 0; i < 7; i++)
	{
		ObjectType* plane = BuildObjectEntity(0, 1);
		plane->SetPos(Vector3(500.0f, 0.0f, -i * 800.0f));
		if (i != 6)
			plane->setState(2);
		
		allEntities.push_back(plane);
	}

	for (int i = 0; i < 7; i++)
	{
		ObjectType* plane = BuildObjectEntity(0, 3);
		plane->SetPos(Vector3(-500.0f, 0.0f, -i * 800.0f));
		
		if (i != 6)
			plane->setState(2);
		
		allEntities.push_back(plane);
	}



	/*for (int x = 0; x <50; x += 10)
	{
		GameEntity* cubeEntity = BuildCubeEntity(5, Vector3(x,x,0));
		allEntities.push_back(cubeEntity);
	}*/


	//for (int x = 0; x < NUMBEROFPLANES; x++)
	//{
	//	//BuildCubeEntity
	//	//upperPlanes[x] = BuildCubeEntity(10, Vector3(TILE_CENTRE_OFFSET, 0, 0));
	//	//lowerPlanes[x] = BuildCubeEntity(10, Vector3(0, TILE_CENTRE_OFFSET, 0));
	//	//rightPlanes[x] = BuildCubeEntity(10, Vector3(-TILE_CENTRE_OFFSET, 0, 0));
	//	//leftPlanes[x] = BuildCubeEntity(10, Vector3(0, -TILE_CENTRE_OFFSET, 0));

	//	upperPlanes[x] = BuildPlaneEntity(0);
	//	rightPlanes[x] = BuildPlaneEntity(1);
	//	lowerPlanes[x] = BuildPlaneEntity(2);
	//	leftPlanes[x]  = BuildPlaneEntity(3);

	//	allEntities.push_back(upperPlanes[x]);
	//	allEntities.push_back(lowerPlanes[x]);
	//	allEntities.push_back(rightPlanes[x]);
	//	allEntities.push_back(leftPlanes[x]);
	//}
	

	/*GameEntity* cubeEntity2 = BuildCubeEntity(5, Vector3(10,10,10));
	allEntities.push_back(cubeEntity2);*/

	//GameEntity* sphereEntity = BuildCubeEntity(80,Vector3(20,10,30));
	//allEntities.push_back(sphereEntity);
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

	handlePlanes(msec);
}

/*
Makes a cube. Every game has a crate in it somewhere!
*/
GameEntity* MyGame::BuildCubeEntity(float size, Vector3 pos) 
{
	SceneNode* s = new SceneNode();
	s->SetMesh(sphere); //tell it to draw our new quad
	//s->SetTransform(Matrix4::scale(Vector3(size, size, size))* Matrix4::translation(pos));
	s->SetModelScale(Vector3(size, size, size));
	
	PhysicsNode* p = new PhysicsNode();
	p->SetPosition(pos);

	GameEntity*g = new GameEntity(s, p);

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


GameEntity* MyGame::BuildPlaneEntity(int position)
{

	SceneNode* s = new SceneNode(cube);
	PhysicsNode* p = new PhysicsNode();

	if (position == 0)//Top
	{
		std::cout << 0 << std::endl;
		s->SetModelScale(Vector3(TILE_WIDTH, TILE_HEIGHT, TILE_DEPTH));
		p->SetPosition(Vector3(0, TILE_CENTRE_OFFSET, 0));
	}

	if (position == 1)//Bottom
	{
		std::cout << 1 << std::endl;
		s->SetModelScale(Vector3(TILE_WIDTH, TILE_HEIGHT, TILE_DEPTH));
		p->SetPosition(Vector3(TILE_CENTRE_OFFSET, 0, 0));
		Vector4 rotVec = MathHelper::vec3ToEuler(0, 0, 90);
		p->SetOrientation(Quat(rotVec.getX(), rotVec.getY(), rotVec.getZ(), rotVec.getW()));
	}
	if (position == 2)//Right
	{
		std::cout << 2 << std::endl;
		s->SetModelScale(Vector3(TILE_WIDTH, TILE_HEIGHT, TILE_DEPTH));
		p->SetPosition(Vector3(0, -TILE_CENTRE_OFFSET, 0));
	}
	if (position == 3)//Left
	{
		std::cout << 3 << std::endl;
		s->SetModelScale(Vector3(TILE_WIDTH, TILE_HEIGHT, TILE_DEPTH));
		p->SetPosition(Vector3(-TILE_CENTRE_OFFSET, 0, 0));
		Vector4 rotVec = MathHelper::vec3ToEuler(0, 0, 90);
		p->SetOrientation(Quat(rotVec.getX(), rotVec.getY(), rotVec.getZ(), rotVec.getW()));
	}

	//p->SetPosition(Vector3(0, 0, 0));
	GameEntity* g = new GameEntity(s, p);
	g->ConnectToSystems();
	return g;
}

GameEntity* MyGame::BuildBulletEntity(float radius, Vector3 pos)
{
	SceneNode* test = new SceneNode(sphere);
	test->SetModelScale(Vector3(radius, radius, radius));
	test->SetBoundingRadius(radius);
	test->SetColour(Vector4(0.2f, 0.2f, 0.5f, 1.0f));
	PhysicsNode*p = new PhysicsNode();
//	p->SetUseDamping(false);
	p->SetUseGravity(false);
	p->SetPosition(pos);
	p->SetCollisionVolume(new CollisionSphere(radius));    //new 4.2.2015  Daixi

	GameEntity*g = new GameEntity(test, p);
	g->ConnectToSystems();
	return g;
}


ObjectType* MyGame::BuildObjectEntity(int type, int subType)
{
	SceneNode* s = new SceneNode(cube);
	if (subType == 0){
		s = new SceneNode(cubeAir);
		s->SetModelScale(Vector3(TILE_WIDTH, TILE_HEIGHT, TILE_DEPTH));

	}
	else if (subType == 1){
		s = new SceneNode(cubeFire);
		s->SetModelScale(Vector3(TILE_WIDTH, TILE_HEIGHT, TILE_DEPTH));

	}
	else if (subType == 2){
		s = new SceneNode(cubeEarth);
		s->SetModelScale(Vector3(TILE_WIDTH, TILE_HEIGHT, TILE_DEPTH));

	}
	else if (subType == 3){

		s = new SceneNode(cubeWater);
		s->SetModelScale(Vector3(TILE_WIDTH, TILE_HEIGHT, TILE_DEPTH));

	}

	PhysicsNode* p = new PhysicsNode();
	p->SetUseGravity(false);

	ObjectType*g = new ObjectType(s, p, type, subType);
	g->ConnectToSystems();
	SceneNode &test = g->GetRenderNode();

	test.SetModelScale(Vector3(TILE_WIDTH, TILE_HEIGHT, TILE_DEPTH));
	test.SetBoundingRadius(TILE_DEPTH);

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
	while (reference[subType].size() > 0)
	{
		reference[subType].pop_back();
	}
	reference[subType].push_back(g);

	return g;
}

Obstacle* MyGame::BuildObstacleEntity(float size, int type, int subType, ObjectType* _obj, int _obstacle_type)
{
	SceneNode* s = new SceneNode(cube);
	PhysicsNode* p = new PhysicsNode();
	p->SetUseGravity(false);

	Obstacle *g = new Obstacle(_obj, s, p, type, subType, _obstacle_type);
	g->SetSize((int)size);
	g->ConnectToSystems();
	SceneNode &test = g->GetRenderNode();

	test.SetModelScale(Vector3(size, size, size));
	test.SetBoundingRadius(size);

	obstacleElements[subType].push_back(g);

	return g;
}

//creates a new Obstacle
//Position defines the plane its on 0-top, 1-bottom, 2-right, 3-left
GameEntity* MyGame::BuildNewObstacleEntity(int position)
{
	SceneNode* s = new SceneNode(sphere);
	PhysicsNode* p = new PhysicsNode();
	s->SetModelScale(Vector3(5, 5, 5));

	if (position == 0)//Top
	{
		p->SetPosition(Vector3(0, TILE_CENTRE_OFFSET, 0));
	}
	else if (position == 1)//Bottom
	{
		p->SetPosition(Vector3(0, -TILE_CENTRE_OFFSET, 0));
	}
	else if (position == 2)//Right
	{
		p->SetPosition(Vector3(TILE_CENTRE_OFFSET, 0, 0));
	}
	else if (position == 3)//Left
	{
		p->SetPosition(Vector3(-TILE_CENTRE_OFFSET, 0, 0));
	}

	GameEntity* g = new GameEntity(s, p);
	return g;

}

GameEntity* MyGame::BuildBuffEntity(float radius, Vector3 pos)
{   //new 6.2.2015 Daixi
	SceneNode* test = new SceneNode(sphere);
	test->SetModelScale(Vector3(radius, radius, radius));
	test->SetBoundingRadius(radius);
	test->SetColour(Vector4(0.2f, 0.2f, 0.5f, 1.0f));
	PhysicsNode*p = new PhysicsNode();

	p->SetUseGravity(false);
	p->SetPosition(pos);

	GameEntity*g = new GameEntity(test, p);
	g->ConnectToSystems();
	return g;
}


void MyGame::CreateObstacle(ObjectType* _obj)
{
	int random_number = rand() % 2 + 1;
	Obstacle* temp = NULL;
	//select random obstacle 

	int obstacleType = rand() % 5 + 0;
	int empty = getObstacleEmptyIndex(_obj->getSubType(), obstacleType);
	// for the first obstacle created
	if (obstacleReference[_obj->getSubType()] == NULL)
	{
		temp = BuildObstacleEntity(25, 1, _obj->getSubType(), _obj, obstacleType);
		if (temp->getObstacleType() == 1)
		{
			/*GameEntity* bul = BuildBulletEntity(20, temp->GetPhysicsNode().GetPosition());
			temp->SetBullet(bul);
			temp->SetPlayer(PhysicsSystem::GetPhysicsSystem().GetPlayer());
			allEntities.push_back(bul);*/

		}
		obstacleElements[_obj->getSubType()].push_back(temp);
	}
	//reference exists, but everything is running/working
	else if (empty == -1)
	{
		if (obstacleReference[_obj->getSubType()] != NULL)
		{
			if (obstacleReference[_obj->getSubType()]->GetPhysicsNode().GetPosition().getZ() > -4750.0f)
			{
				temp = BuildObstacleEntity(25, 1, _obj->getSubType(), _obj, obstacleType);
				if (temp->getObstacleType() == 1)
				{
					/*GameEntity* bul = BuildBulletEntity(20, temp->GetPhysicsNode().GetPosition());
					temp->SetBullet(bul);
					temp->SetPlayer(PhysicsSystem::GetPhysicsSystem().GetPlayer());
					allEntities.push_back(bul);*/

				}
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
			if (obstacleReference[_obj->getSubType()]->GetPhysicsNode().GetPosition().getZ() > -4750.0f)
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
	for (unsigned int i = 0; i < obstacleElements[_subType].size(); i++)
	{
		if (obstacleElements[_subType][i] != NULL)
		if (obstacleElements[_subType][i]->getState() == 2 && _obstacleType == obstacleElements[_subType][i]->getObstacleType())
		{
			return i;
		}
	}
	return -1;
}

void MyGame::handlePlanes(float msec)
{
	for (int i = 0; i < 4; i++)
	{
		// iterate through all obstacles
		for (unsigned int j = 0; j < obstacleElements[i].size(); j++)
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
		for (unsigned int j = 0; j < elements[i].size(); j++)
		{
			if (elements[i][j]->getState() == 1)
			{
				//create new
				if (getEmptyIndex(i) == -1)
				{
					allEntities.push_back(BuildObjectEntity(0, i));
				}
				else
				{
					int x = getEmptyIndex(i);

					elements[i][x]->reset();
					

					while (reference[i].size()>0) //Clear Any possible references to tiles
						reference[i].pop_back();

					reference[i].push_back(elements[i][x]); //Add the last tile to be used to the references vector
					
					
					if ((rand() % 100 + 1) > 50)
					{
						if (getDrawingPlanes(i) > 0)
						{
							elements[i][x]->setRandom(rand() % 5 + 1);
						}
					}

					allEntities.push_back(elements[i][x]);

					//create obstacle
					if ((rand() % 100 + 1) < 50)
					{
						CreateObstacle(elements[i][x]);
					}
						
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


int MyGame::getIndexOfAllEtities(GameEntity* _G)
{
	int index = -1;
	for (unsigned int i = 0; i < allEntities.size(); i++)
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
	for (unsigned int j = 0; j < elements[_G->getSubType()].size(); j++)
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
	for (unsigned int i = 0; i < elements[_subType].size(); i++)
	{
		if (elements[_subType][i]->getState() == 4)
		{
			return i;
		}
	}
	return -1;
}