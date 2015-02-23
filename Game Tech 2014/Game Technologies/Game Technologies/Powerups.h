#pragma once
#include"MyGame.h"

class Vehicle;
class Coins;
class Powerups {
public:
	Powerups();
	~Powerups(void);
	void UpdatePowerup(Vehicle* Car, Coins* coins, float msec);
	GameEntity* BuildPowerupEntity(float size, Vector3 pos);
	GameEntity* GetTime_warp(){return Time_warp;}
	GameEntity* GetSpeed_boost(){return Speed_boost;}
	GameEntity* GetInvulnerability(){return Invulnerability;}
	GameEntity* GetPickup_magnet(){return Pickup_magnet;}
	Vector3 GetSpeedboostPosition(){return SpeedboostPosition;}
	void SetSpeedboostPosition(Vector3 pos){SpeedboostPosition = pos;}
	Vector3 GetInvulnerabilityPosition(){return InvulnerabilityPosition;}
	void SetInvulnerabilityPosition(Vector3 pos){InvulnerabilityPosition = pos;}
	void TestPosition(){}

	Vector3 tempPosition;
protected:
	Vector3 SpeedboostPosition;
	Vector3 InvulnerabilityPosition;
	Vector3 PickupmagnetPosition;
	Vector3 temp,temp1,temp2,temp3; 
	Vector4 ColorTemp;

	Mesh* PowerupMesh;
	GameEntity* Time_warp; 
	GameEntity* Speed_boost; 
	GameEntity* Invulnerability; 
	GameEntity* Pickup_magnet; 

	float TestLength;
	float size;
	float SpeedboostTime;
	float InvulnerabilityTime;
	float PickupmagnetTime;
};