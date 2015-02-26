#pragma once
#include "MyGame.h"
//#include "GameEntity.h"
#include <time.h>
class Vehicle;
class Coins {
public:
	Coins(int coinsnum);
	~Coins(void);
	GameEntity* BuildCoinsEntity(float radius);
	void UpdateCoins(Vehicle* car);
	void UpdatePickupCoins(Vehicle* car);
	int GetCoinsnum(){ return Coins_num; }
	vector<GameEntity*> GetAllcoins(){ return Allcoins; }
protected:
	Vector3 CoinsPosition;
	Vector3 temp, temp1, temp2, temp3;
	Vector4 colourtemp1, colourtemp2;
	float random_num0;
	float random_num1;
	Mesh* CoinsMesh;
	vector<GameEntity*> Allcoins;
	int Coins_num;
	int coin_size;
	float Test_length;
};