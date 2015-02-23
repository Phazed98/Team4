#pragma once

#include "../../nclgl/Quaternion.h"
#include "../../nclgl/Vector3.h"
#include "../../nclgl/Matrix4.h"
#include "../../nclgl/SceneNode.h"

#include "CollisionVolume.h"

#define LINEAR_VELOCITY_DAMP 0.998
#define ANGULAR_VELOCITY_DAMP 0.998
#define LINEAR_VELOCITY_MIN 0.00001
#define RESTCHECKFRAMES 12

class PhysicsNode	{
public:
	PhysicsNode(void);
	PhysicsNode(Quaternion orientation, Vector3 position);
	~PhysicsNode(void);

	Vector3		GetPosition() const			{ return m_position;}
	Vector3		GetLinearVelocity() const	{ return m_linearVelocity;}
	
	float		GetInverseMass() const		{return m_invMass;}
	float		GetMass() const				{return 1.0f/m_invMass;}
	float		GetElasticity() const		{ return m_elasticity; }

	Matrix4		GetInverseInertiaMatrix() const	{ return m_invInertia; }

	Quaternion	GetOrientation() const		{ return m_orientation;}
	Vector3		GetAngularVelocity() const	{ return m_angularVelocity;}

	SceneNode*			getTarget()				const	{ return target; }

	CollisionVolume* GetCollisionVolume() const { return vol; }

	Matrix4		BuildTransform();

	virtual void		Update(float msec);
	void		updateLinear(float msec);
	void		updateAngular(float msec);
	void		CalculateAvgVel();

	void	SetPosition(Vector3 vec) { m_position = vec; }
	void	SetLinearVelocity(Vector3 vec)		{ m_linearVelocity = vec;}
	
	void	SetMass(float mass) {m_invMass = 1.0f/mass;}
	void	SetInverseMass(float invMass) {m_invMass = invMass;}

	void	SetInverseInertia(Matrix4 invInertia) {m_invInertia = invInertia;}
	
	void	SetOrientation(Quaternion q) { m_orientation = q; }
	void	SetAngularVelocity(Vector3 vec) { m_angularVelocity = vec; }

	void	SetTarget(SceneNode *s) { target = s;}
	void	SetUseGravity(bool value) { useGravity = value; }
	void	SetCollisionVolume(CollisionVolume* vol) { this->vol = vol; }
	void	SetElasticity(float value){ m_elasticity = value; }


	Vector3	GetForce()	{ return m_force;}
	Vector3	GetTorque() { return m_torque;}

	void AddForce(Vector3 point, Vector3 force);

	static const Vector3 gravity;

	//------------------------------------------
	//Added by Sam for physics
	//sets the AABBHalfLenth by pulling data from the target scene node mesh
	void ConfigureAABBHalfLength();

	float GetAABBHalfLength() { return AABBHalfLength; }

	void SetMovable(bool flag) { m_moveable = flag; }
	//-----------------------------------------------

protected:
	bool useGravity;
	bool m_moveable;
	bool m_atRest;
	float m_elasticity;

	//<---------LINEAR-------------->
	Vector3		m_position;
	Vector3		m_linearVelocity;
	Vector3		m_force;
	float		m_invMass;
	Vector3 m_avgVelocity;
	Vector3 avgVel[RESTCHECKFRAMES];

	//<----------ANGULAR--------------->
	Quaternion  m_orientation;
	Vector3		m_angularVelocity;
	Vector3		m_torque;
	Matrix4     m_invInertia;

	SceneNode*	target;
	CollisionVolume* vol;


	//--------------------------------------------------------
	//Added by Sam for physics
	//Repressents the AABB for the shape for broadphase
	float AABBHalfLength;
	//----------------------------------------------------------
};

