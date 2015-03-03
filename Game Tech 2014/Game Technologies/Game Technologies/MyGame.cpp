#include "MyGame.h"
#include "InertialMatrixHelper.h"
#include "Spring.h"
#include "SpringDemo.h"
#include "NetworkClient.h"
#include "Powerups.h"
#include "Coins.h"
#include "ChaseCamera.h"


/*
Creates a really simple scene for our game - A cube robot standing on
a floor. As the module progresses you'll see how to get the robot moving
around in a physically accurate manner, and how to stop it falling
through the floor as gravity is added to the scene. 

You can completely change all of this if you want, it's your game!
*/

unsigned int MyGame::client_id;

MyGame::MyGame(bool isHost, bool isClient, bool useNetworking, int numClients)
{

	this->isHost = isHost;
	this->isClient = isClient;
	this->useNetworking = useNetworking;
	this->numClients = numClients;

	if (useNetworking)
	{
		if (isHost)
		{
			//Init client list to nothing
			client_id = 0;

			//Spawn Server Which listens for clients
			networkServer = new NetworkServer();
		}

		if (isClient)
		{
			//Spawn a New Client which will attempt to connect to a server
			networkClient = new NetworkClient();

			// send init packet
			const unsigned int packet_size = sizeof(Packet);
			char packet_data[packet_size];

			Packet packet;
			packet.packet_type = INIT_CONNECTION;
			packet.serialize(packet_data);
			NetworkServices::sendMessage(networkClient->ConnectSocket, packet_data, packet_size);
		}
	}


	Renderer::GetRenderer().RenderLoading(0, "Initializing...");
	setCurrentState(Game_LOADING);
	timer = 0;

	count_time = 0;    //new control shoot the bullets   4.2.2015 Daixi

	Speed_Player = 1;  //control the player speed

	Renderer::GetRenderer().RenderLoading(5, "Loading Elements...");

	elements.push_back(top);
	elements.push_back(right);
	elements.push_back(bottom);
	elements.push_back(left);

	Renderer::GetRenderer().RenderLoading(15,"Feeding Hamsters..." );

	reference.push_back(top);
	reference.push_back(right);
	reference.push_back(bottom);
	reference.push_back(left);

	Renderer::GetRenderer().RenderLoading(25,"Creating Obtsacles...");

	obstacleElements.resize(4);
	obstacleReference.resize(4);

	Renderer::GetRenderer().RenderLoading(30,"Referencing Obstacles..."); 
	for (int i = 0; i < 4; i++)
	{
		obstacleReference[i] = NULL;
	}
	Renderer::GetRenderer().RenderLoading(40,"Attaching Mooses...");

	Vehicle* player = PhysicsSystem::GetVehicle();
	gameCamera = new ChaseCamera(player, 200, 20, 12);


	Renderer::GetRenderer().RenderLoading(50, "I knew A Moose Once...");

	Renderer::GetRenderer().SetCamera(gameCamera);

	Renderer::GetRenderer().RenderLoading(70, "He was a really good moose...");

	Renderer::GetRenderer().RenderLoading(75, "The moose never bit anyone...");
	Renderer::GetRenderer().RenderLoading(76, "I called him George...");
	Renderer::GetRenderer().RenderLoading(91, "Sorry about that...");
	Renderer::GetRenderer().RenderLoading(92, "The person responsible for the loading screen...");
	Renderer::GetRenderer().RenderLoading(93, "...has been sacked...");

	//add all powerup variable!!!
	//AllCoins = new Coins(10);
	//AllPowerups = new Powerups();

	cube	= new OBJMesh(MESHDIR"cube.obj");
	cubeAir = new OBJMesh(MESHDIR"cube.obj");
	cubeAir->SetTexture(airTexture);
	cubeWater = new OBJMesh(MESHDIR"cube.obj");
	cubeWater->SetTexture(waterTexture);
	cubeFire = new OBJMesh(MESHDIR"cube.obj");
	cubeFire->SetTexture(fireTexture);
	cubeEarth = new OBJMesh(MESHDIR"cube.obj");
	cubeEarth->SetTexture(earthTexture);
	quad	= Mesh::GenerateQuad();
	sphere	= new OBJMesh(MESHDIR"ico.obj");
	Renderer::GetRenderer().RenderLoading(95, "Lammas are nice creatures...");
	Renderer::GetRenderer().RenderLoading(96,"Hold it,  Were not getting into Lammas now...");
	Renderer::GetRenderer().RenderLoading(97, "The loading system is obviously faulty, we will continue now without any further updates...");
	Renderer::GetRenderer().RenderLoading(98, "Except this to say we love Mooses? Meese? Multiple Moose? ...");

	//-------------------------------------------------Planes---------------------------------------------------------//

	for (int i = 0; i < 7; i++)
	{
		ObjectType* plane = BuildObjectEntity(0, 2);
		plane->SetPos(Vector3(0.0f, -TILE_CENTRE_OFFSET, -i * 800.0f));
		if (i != 6)
			plane->setState(2);
		allEntities.push_back(plane);
	}

	for (int i = 0; i < 7; i++)
	{
		ObjectType* plane = BuildObjectEntity(0, 0);
		plane->SetPos(Vector3(0.0f, TILE_CENTRE_OFFSET, -i * 800.0f));
		if (i != 6)
			plane->setState(2);
		allEntities.push_back(plane);
	}

	for (int i = 0; i < 7; i++)
	{
		ObjectType* plane = BuildObjectEntity(0, 1);
		plane->SetPos(Vector3(TILE_CENTRE_OFFSET, 0.0f, -i * 800.0f));
		if (i != 6)
			plane->setState(2);
		allEntities.push_back(plane);
	}

	for (int i = 0; i < 7; i++)
	{
		ObjectType* plane = BuildObjectEntity(0, 3);
		plane->SetPos(Vector3(-TILE_CENTRE_OFFSET, 0.0f, -i * 800.0f));
		if (i != 6)
			plane->setState(2);
		allEntities.push_back(plane);
	}

	allEntities.push_back(BuildCheckPointEntity(2, 4, 200));

	setCurrentState(GAME_PLAYING);
	Renderer::GetRenderer().RenderLoading(100, "Done...");
	
	
	if (useNetworking)
	{
		if (isHost)
		{
			Renderer::GetRenderer().RenderLoading(100, "Waiting on Clients");
			while (client_id < numClients)
			{
				// get new clients
				if (networkServer->acceptNewClient(client_id))
				{
					std::cout << "Client " << client_id << " has been connected to the server." << std::endl;
					client_id++;
				}
			}
			sendServerStartPackets();
		}

		if (isClient)
		{
			bool startGame = false;
			while (!startGame)
			{
				if (!isHost)
					Renderer::GetRenderer().RenderLoading(100, "Waiting on Host");

				Packet packet;
				int data_length = networkClient->receivePackets(client_network_data);

				if (data_length <= 0)
				{
					continue;
				}
				unsigned int i = 1;
				while (i < (unsigned int)data_length)
				{
					packet.deserialize(client_network_data);
					i += sizeof(Packet);

					if (packet.packet_type == START_GAME)
					{
						startGame = true;
						srand(packet.packet_integer);
						break;
					}
				}
			}
		}
	}
	else
	{
		srand((int)Window::GetWindow().GetTimer()->GetMS());
	}
}

MyGame::~MyGame(void)	
{
	/*
	We're done with our assets now, so we can delete them
	*/
	delete cube;
	delete quad;
	delete sphere;
//	delete Enemy;   //new 5.2.2015 Daixi
	delete Car;
	delete bullet;
	delete AllCoins;
	delete AllPowerups;

	CubeRobot::DeleteCube();

	elements.clear();
	reference.clear();
	obstacleElements.clear();
	obstacleReference.clear();

	//GameClass destructor will destroy your entities for you...
	setCurrentState(GAME_EXIT);
}

/*
Here's the base 'skeleton' of your game update loop! You will presumably
want your games to have some sort of internal logic to them, and this
logic will be added to this function.
*/
void MyGame::UpdateGame(float msec) 
{
	if (currentGameState != GAME_PLAYING)
		return;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_L)) //Build Cube
	{
		PhysicsSystem::SetTrackSpeed(PhysicsSystem::GetTrackSpeed() + 1);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_K)) //Build Cube
	{
		PhysicsSystem::SetTrackSpeed(PhysicsSystem::GetTrackSpeed() - 1);
	}

	if(gameCamera) 
	{
		gameCamera->UpdateCamera(msec);
	}

	for(vector<GameEntity*>::iterator i = allEntities.begin(); i != allEntities.end(); ++i) 
	{
		(*i)->Update(msec);
	}


	Car = PhysicsSystem::GetPhysicsSystem().GetVehicle();
	/*
	AllPowerups->UpdatePowerup(Car, AllCoins, msec);
	AllCoins->UpdateCoins(Car);
	AllCoins->UpdatePickupCoins(Car);
	*/

	timer += 0.001f;

	handlePlanes(timer);
	//handleNetworking();
}

GameEntity* MyGame::BuildPlayerEntity(float size, Vector3 pos)
{
	SceneNode* test = new SceneNode(PlayerMesh);
	test->SetModelScale(Vector3(size, size, size));
	test->SetBoundingRadius(size);
	test->SetColour(Vector4(0.2f, 0.2f, 0.5f, 1.0f));
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
	test->SetColour(Vector4(0.2f, 0.2f, 0.5f, 1.0f));
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
GameEntity* MyGame::BuildCubeEntity(float size)
{
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

ObjectType* MyGame::BuildObjectEntity(int type, int subType) 
{
	SceneNode* s = new SceneNode(cube);
	if (subType == 0){
		s = new SceneNode(cubeAir);
	}
	else if (subType == 1){
		s = new SceneNode(cubeFire);

	}
	else if (subType == 2){
		s = new SceneNode(cubeEarth);

	}
	else if (subType == 3){
		
		s = new SceneNode(cubeWater);

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
	while(reference[subType].size() > 0)
	{
		reference[subType].pop_back();
	}
	reference[subType].push_back(g);
	
	return g;
}

//creates a new Obstacle
Obstacle* MyGame::BuildObstacleEntity(float size, int type, int subType, ObjectType* _obj, int _obstacle_type) 
{
	SceneNode* s;
	if (_obstacle_type == 5)
	{
		s = new SceneNode(cube);
	}
	else if (subType == 0 && _obstacle_type == 4)
	{
		s = new TornadoSceneNode(sphere);
		Renderer::GetRenderer().SetTornadoSceneNode((TornadoSceneNode*)s);
		size = size / size;
	}
	else
	{
		s = new SceneNode(sphere);
	}
	
	PhysicsNode* p = new PhysicsNode();
	p->SetUseGravity(false);

	Obstacle *g = new Obstacle(_obj, s, p, type, subType, _obstacle_type);
	g->SetSize((int)size);
	g->ConnectToSystems();
	if (_obstacle_type == 5)
	{
		SceneNode &test = g->GetRenderNode();

		test.SetModelScale(Vector3(TILE_WIDTH, TILE_HEIGHT * 2, 20));
		test.SetBoundingRadius(TILE_DEPTH);
	}
	else
	{
		SceneNode &test = g->GetRenderNode();

		test.SetModelScale(Vector3(size, size, size));
		test.SetBoundingRadius(size);
	}
	

	obstacleElements[subType].push_back(g);

	return g;
}

GameEntity* MyGame::BuildBulletEntity(float radius, Vector3 pos)
{
	SceneNode* test = new SceneNode(sphere);
	test->SetModelScale(Vector3(radius, radius, radius));
	test->SetBoundingRadius(radius);
	test->SetColour(Vector4(0.2f, 0.2f, 0.5f, 1.0f));
	PhysicsNode*p = new PhysicsNode();
	p->SetUseDamping(false);
	p->SetUseGravity(false);
	p->SetPosition(pos);
	p->SetCollisionVolume(new CollisionSphere(radius));    //new 4.2.2015  Daixi

	GameEntity*g = new GameEntity(test, p);
	g->ConnectToSystems();
	return g;
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
					if ((rand() % 100 + 1) < 50)
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
	//select random obstacle 

	int obstacleType = rand() % 6 + 0;
	int empty = getObstacleEmptyIndex(_obj->getSubType(),obstacleType);
	// for the first obstacle created
	if (obstacleReference[_obj->getSubType()] == NULL)
	{
		temp = BuildObstacleEntity(25, 1, _obj->getSubType(), _obj, obstacleType);
		if (temp->getObstacleType() == 1)
		{
			GameEntity* bul = BuildBulletEntity(20, temp->GetPhysicsNode().GetPosition());
			temp->SetBullet(bul);
			temp->SetPlayer(PhysicsSystem::GetPhysicsSystem().GetPlayer());
			allEntities.push_back(bul);

		}
		obstacleElements[_obj->getSubType()].push_back(temp);
	}
	//reference exists, but everything is running/working
	else if (empty == -1)
	{
		if (obstacleReference[_obj->getSubType()] != NULL)
		{
			if (obstacleReference[_obj->getSubType()]->GetPhysicsNode().GetPosition().z > -4750.0f)
			{
				temp = BuildObstacleEntity(25, 1, _obj->getSubType(), _obj, obstacleType);
				if (temp->getObstacleType() == 1)
				{
					GameEntity* bul = BuildBulletEntity(20, temp->GetPhysicsNode().GetPosition());
					temp->SetBullet(bul);
					temp->SetPlayer(PhysicsSystem::GetPhysicsSystem().GetPlayer());
					allEntities.push_back(bul);

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
			if (obstacleReference[_obj->getSubType()]->GetPhysicsNode().GetPosition().z > -4750.0f)
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
		if (obstacleElements[_subType][i]!=NULL)
		if (obstacleElements[_subType][i]->getState() == 2 && _obstacleType == obstacleElements[_subType][i]->getObstacleType())
		{
			return i;
		}
	}
	return -1;
}

void MyGame::receiveFromClients()
{
	Packet packet;

	// go through all clients
	std::map<unsigned int, SOCKET>::iterator iter;
	int i = 0;
	for (iter = networkServer->sessions.begin(); iter != networkServer->sessions.end(); iter++)
	{
		int data_length = networkServer->receiveData(iter->first, server_network_data);

		if (data_length <= 0)
		{
			//no data recieved
			continue;
		}

		unsigned int i = 1;
		while (i < (unsigned int)data_length)
		{
			packet.deserialize(&(server_network_data[i]));
			i += sizeof(Packet);

			switch (packet.packet_type)
			{

			case INIT_CONNECTION:

				cout << "server received init packet from client" << endl;
				sendServerActionPackets();

				break;

			case ACTION_EVENT:

				cout << "server received action event packet from client" << endl;
				sendServerActionPackets();
				break;

			default:
				cout << "error in packet types" << endl;

				break;
			}
			++i;
		}
	}
}


void MyGame::sendToClients()
{
	Packet packet;
	// go through all clients
	std::map<unsigned int, SOCKET>::iterator iter;

	for (iter = networkServer->sessions.begin(); iter != networkServer->sessions.end(); iter++)
	{
		int data_length = networkServer->receiveData(iter->first, server_network_data);

		if (data_length <= 0)
		{
			//no data recieved
			continue;
		}

		unsigned int i = 0;
		while (i < (unsigned int)data_length)
		{
			packet.deserialize(&(server_network_data[i]));
			i += sizeof(Packet);

			switch (packet.packet_type)
			{

			case INIT_CONNECTION:

				cout << "Server received init packet from client." << endl;

				//sendServerActionPackets();

				break;

			case ACTION_EVENT:

				cout << "Server received action event packet from client." << endl;

				//sendServerActionPackets();

				break;

			default:

				cout << "Error in packet types." << endl;

				break;
			}
		}
	}
}


void MyGame::sendServerActionPackets()
{
	// send action packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = ACTION_EVENT;
	packet.packet_integer = 0;
	packet.serialize(packet_data);

	networkServer->sendToAll(packet_data, packet_size);
}

void MyGame::sendServerStartPackets()
{
	// send action packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = START_GAME;
	packet.packet_integer = (int)Window::GetWindow().GetTimer()->GetMS();
	packet.serialize(packet_data);

	networkServer->sendToAll(packet_data, packet_size);
}


void MyGame::sendClientActionPackets()
{
	// send action packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = ACTION_EVENT;
	packet.packet_integer = 0;
	packet.serialize(packet_data);

	NetworkServices::sendMessage(networkClient->ConnectSocket, packet_data, packet_size);
}

void MyGame::handleNetworking()
{
	if (IS_HOST)
	{
		// get new clients
		if (networkServer->acceptNewClient(client_id))
		{
			cout << "Client " << client_id << " has been connected to the server." << endl;
			client_id++;
		}
		sendServerActionPackets();
	}


	if (IS_CLIENT)
	{
		Packet packet;
		int data_length = networkClient->receivePackets(client_network_data);

		if (data_length <= 0)
		{
			return;
		}

		unsigned int i = 0;
		while (i < (unsigned int)data_length)
		{
			packet.deserialize(&(client_network_data[i]));
			i += sizeof(Packet);

			switch (packet.packet_type)
			{
			case ACTION_EVENT:
				break;

			case INIT_CONNECTION:
				break;

			case START_GAME:
				break;

			default:
				cout << "error in packet types" << endl;
				break;
			}
		}
	}
}

ObjectType* MyGame::BuildCheckPointEntity(int type, int subType, int size)
{
	SceneNode* s = new SceneNode(quad);

	s->SetModelScale(Vector3(size, size, size));
	//Oh if only we had a set texture function...we could make our brick floor again WINK WINK
	s->SetBoundingRadius(size);

	PhysicsNode*p = new PhysicsNode(Quaternion::AxisAngleToQuaterion(Vector3(1, 0, 0), 180.0f), Vector3());
	p->SetUseGravity(false);
	p->SetInverseMass(0.0f);
	p->SetInverseInertia(InertialMatrixHelper::createImmovableInvInertial());
	p->SetCollisionVolume(new CollisionPlane(Vector3(0, 1, 0), 0));

	ObjectType*g = new ObjectType(s, p, type, subType);
	g->ConnectToSystems();




	return g;
}