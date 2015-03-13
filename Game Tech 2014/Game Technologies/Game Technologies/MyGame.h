#pragma once
#include "GameEntity.h"
#include "ObjectType.h"
#include "Obstacle.h"
#include "GameClass.h"
#include "../nclgl/Camera.h"
#include "../nclgl/CubeRobot.h"
#include "Vehicle.h"
#include "Bullets.h"
#include "NetworkClient.h"
#include "NetworkServer.h"
#include "NetworkData.h"
#include "TornadoSceneNode.h"
#include "FireSceneNode.h"
#include "GeyserSceneNode.h"
#include "LifeformSceneNode.h"


struct sendData
{
	int resetNumber;
	int randomPlane;
	int createObstacleNumber;
	int objectRandValue;
	int objectRandPlane;
	int obstacleRandNum;
};


class Coins;
class Powerups;
class Vehicle;
class Bullets;
class MyGame : public GameClass	
{
public:
//	MyGame();
	MyGame(bool isHost, bool isClient, bool useNetworking, int numClients);
	~MyGame(void);

	virtual void UpdateGame(float msec);

	int getIndexOfAllEtities(GameEntity* _G);

	int getIndexOfElements(ObjectType* _G);

	int getDrawingPlanes(int);

	int getEmptyIndex(int subType);

	void handlePlanes(float msec);
	void handlePlanesNetwork(int resetNumber, int randomPlane, int createObstacleNumber, int objectRandValue, int objectRandPlane, int obstacleRandNum);

	void handleNetworking();

	void CreateObstacle(ObjectType* _obj);
	void CreateObstacleNetwork(ObjectType* _obj, int random_number);

	int getObstacleEmptyIndex(int _subType,int);

	void UpdatePlayer(float msec);

	void ShootBullets();  //new 4.2.2015 Daixi

	/*---------------------
	NETWORKING METHODS
	Matthew Battison
	---------------------*/
	//Server
	void receiveFromClients();
	void sendToClients();
	void sendServerActionPackets();
	void sendServerStartPackets();
	void sendServerPausePackets();
	void sendServerUnPausePackets();
	void sendServerAssignPackets(int client);
	void sendServerUpdatePackets(int client);
	char server_network_data[MAX_PACKET_SIZE];

	//Client
	void sendClientActionPackets();
	void sendClientUpdatePackets();
	void sendClientPausePackets();
	void sendClientUnPausePackets();
	void recieveFromServer();
	char client_network_data[MAX_PACKET_SIZE];



protected:
	GameEntity* BuildRobotEntity();

	GameEntity* BuildCubeEntity(float size);

	GameEntity* BuildSphereEntity(float radius, Vector3 pos, Vector3 vel);

	GameEntity* BuildBulletEntity(float radius, Vector3 pos);

	GameEntity* BuildQuadEntity(float size);

	ObjectType* BuildObjectEntity(int type, int subType);
	ObjectType* BuildObjectEntityNetwork(int type, int subType, int setRandValue, int randPlane);

	Obstacle* BuildObstacleEntity(float size, int type, int subType, ObjectType*, int);

	GameEntity* BuildBuffEntity(float radius, Vector3 pos); //new 6.2.2015 Daixi

	GameEntity* BuildPlayerEntity(float size, Vector3 pos);

	ObjectType* BuildCheckPointEntity(int type, int subType, int size);

	Mesh* cube;
	Mesh* cubeWater;
	Mesh* cubeAir;
	Mesh* cubeFire;
	Mesh* cubeEarth;

	Mesh* quad;
	Mesh* sphere;
	Mesh* bigRock;
	Mesh* missile;

	

	vector<ObjectType*> top;
	vector<ObjectType*> right;
	vector<ObjectType*> bottom;
	vector<ObjectType*> left;

	vector<vector<ObjectType*>> elements;
	vector<vector<ObjectType*>> reference;

	vector<vector<Obstacle*>> obstacleElements;
	vector<Obstacle*> obstacleReference;

	GameEntity* Enemy;  //new 4.2.2015 Daixi  just for test i create some enemy

	GameEntity* BuffEntity;

	Vehicle* Car;

	Bullets* bullet;
	Powerups* AllPowerups;
	Coins* AllCoins;

	Vector3 PositionEnemy;  //new 4.2.2015 Daixi
	Vector3 Position0;
	Vector3 Position1;

	Vector3 temp;   // 5.2.2015 for caculate the length between Player and Buff
	Vector3 temp1, temp2, temp3;  //Control the player speed  6.2.2015

	Mesh* PlayerMesh; //new 3.2.2015 Daixi


	int count_time;  //new 4.2.2015 Daixi
	int Speed_Player; //4.2.2015 Daixi
	
	float timer;

	/*
	NETWORKING OBJECTS - MATTHEW BATTISON
	*/
	static unsigned int client_id;
	NetworkServer* networkServer;
	NetworkClient* networkClient;
	Matrix4 playerPositions[4];

	bool isHost, isClient, useNetworking;
	int numClients;
	GameEntity* serverPlayers[4];
	GameEntity* players[4];
	float offsets[4];
	float serverOffsets[4];
	int clientNumber;

	/*
	NETWORKING OBJECTS - MATTHEW BATTISON
	*/

	GLuint earthTexture = SOIL_load_OGL_texture("../../Textures/planet1.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	GLuint waterTexture = SOIL_load_OGL_texture("../../Textures/05_DIFFUSE.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	GLuint airTexture = SOIL_load_OGL_texture("../../Textures/sky.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	GLuint fireTexture = SOIL_load_OGL_texture("../../Textures/lava_texture.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	GLuint checkTexture = SOIL_load_OGL_texture("../../Textures/Checkpoint.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	GLuint missileTexture = SOIL_load_OGL_texture("../../Textures/missileTex.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	GLuint coinTexture = SOIL_load_OGL_texture("../../Textures/coin.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	OBJMesh* water_sphere;
	bool wasPaused;

	GLuint cloud_texture;
	GLuint fireball_texture;
	GLuint rainbow_texture;
};

