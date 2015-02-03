#include "PhysicsNode.h"

const Vector3 PhysicsNode::gravity = Vector3(0,-0.001,0);

PhysicsNode::PhysicsNode(void): vol(NULL) 
{
	target = NULL;
	useGravity = true;
}

PhysicsNode::PhysicsNode(Quaternion orientation, Vector3 position): vol(NULL) 
{
	m_orientation	= orientation;
	m_position		= position;
	useGravity = true;
}

PhysicsNode::~PhysicsNode(void)	
{

}

//You will perform your per-object physics integration, here!
//I've added in a bit that will set the transform of the
//graphical representation of this object, too.
void	PhysicsNode::Update(float msec) 
{
	//Damping Factor
	m_linearVelocity = m_linearVelocity * 0.98;
	m_angularVelocity = m_angularVelocity * 0.98;

	//Linear and Angular Updates
	updateLinear(msec);
	updateAngular(msec);

	//Removal of Forces
	m_force = Vector3(0,0,0);
	m_torque = Vector3(0,0,0);

	//Set Graphical Representation
	if(target) 
	{
		target->SetTransform(BuildTransform());
	}
}

void PhysicsNode::updateLinear(float msec)
{
	if (m_moveable)
	{
		//Apply Gravitty if it should be applied
		Vector3 accel = m_force*m_invMass + (useGravity ? gravity : Vector3(0, 0, 0));

		m_linearVelocity = m_linearVelocity + accel * msec;
		m_position = m_position + m_linearVelocity * msec;
		m_atRest = false;

		//Adds current Velocity to an Array and determines the average velocity over the last few updates
		CalculateAvgVel();

		//If The Object has been close to zero velocity for several frames set it to rest
		bool stopped = true;
		for (int x = 0; x < RESTCHECKFRAMES; x++)
		{
			if (avgVel[x].LengthSqrd() > 0.00000001)
			{
				stopped = false;
			}
		}

		if (stopped)
		{
			m_angularVelocity = Vector3(0, 0, 0);
			m_linearVelocity = Vector3(0, 0, 0);
			m_atRest = true;
		}
	}
}

void PhysicsNode::updateAngular(float msec)
{
	if (m_moveable)
	{
		Vector3 angAccel = m_invInertia * m_torque;
		m_angularVelocity = m_angularVelocity + angAccel * msec;
		m_orientation = m_orientation + m_orientation * (m_angularVelocity * msec / 2.0f);
		m_orientation.Normalise();
	}
}


void PhysicsNode::AddForce(Vector3 point, Vector3 force) 
{
	m_force = m_force + force;
	m_torque = m_torque + Vector3::Cross(point - m_position, force);
}

void PhysicsNode::CalculateAvgVel()
{
	m_avgVelocity = Vector3(0.0f, 0.0f, 0.0f);
	
	for (int x = 0; x < RESTCHECKFRAMES - 1; x++)
		avgVel[x] = avgVel[x + 1];

	avgVel[RESTCHECKFRAMES - 1] = m_linearVelocity;


	for (int x = 0; x < RESTCHECKFRAMES; x++)
		m_avgVelocity += avgVel[x];

	m_avgVelocity = m_avgVelocity / RESTCHECKFRAMES;
}

/*
This function simply turns the orientation and position
of our physics node into a transformation matrix, suitable
for plugging into our Renderer!

It is cleaner to work with matrices when it comes to rendering,
as it is what shaders expect, and allow us to keep all of our
transforms together in a single construct. But when it comes to
physics processing and 'game-side' logic, it is much neater to
have seperate orientations and positions.
*/

Matrix4		PhysicsNode::BuildTransform() 
{
	Matrix4 m = m_orientation.ToMatrix();
	m.SetPositionVector(m_position);
	return m;
}