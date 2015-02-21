#include "PhysicsNode.h"

const Vector3 PhysicsNode::gravity = Vector3(0, -0.00001, 0);

PhysicsNode::PhysicsNode(void) : vol(NULL)
{
	target = NULL;
	useGravity = true;
	m_elasticity = 1.0f;
}

PhysicsNode::PhysicsNode(Quat orientation, Vector3 position) : vol(NULL)
{
	m_orientation = orientation;
	m_position = position;
	m_elasticity = 1.0f;
	useGravity = true;
}

PhysicsNode::~PhysicsNode(void)
{

}


void	PhysicsNode::Update(float msec)
{
	//std::cout << "Physics Eh?" << std::endl;
	//Damping Factor
	m_linearVelocity = m_linearVelocity * 0.98;
	m_angularVelocity = m_angularVelocity * 0.98;

	//Linear and Angular Updates
	updateLinear(msec);
	updateAngular(msec);

	//Removal of Forces
	m_force = Vector3(0, 0, 0);
	m_torque = Vector3(0, 0, 0);

	//Set Graphical Representation
	if (target)
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
			//inline const floatInVec lengthSqr(Vector3 vec);
			//.LengthSqrd()
			if (lengthSqr(avgVel[x])> 0.00000001)
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
		Vector3 angAccel = m_invInertia.getUpper3x3() * m_torque;
		m_angularVelocity = m_angularVelocity + angAccel * msec;
		/*
		This could be horribly wrong, but i have to try it to get things working, 
		this should be multiplying a quaterion by a vec3 however im useing it as a rotation
		*/
		//m_orientation = m_orientation + m_orientation * (m_angularVelocity * msec / 2.0f);
		rotate(m_orientation, (m_angularVelocity * msec / 2.0f));
		//m_orientation.Normalise();
		normalize(m_orientation);

	}
}


void PhysicsNode::AddForce(Vector3 point, Vector3 force)
{
	m_force = m_force + force;
	m_torque = m_torque + cross(point - m_position, force);
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


Matrix4		PhysicsNode::BuildTransform()
{
	//Matrix4::rotation(m_orientation);
	//Matrix4 m = m_orientation.ToMatrix();
	Matrix4 m = Matrix4::rotation(m_orientation);;
	m.setTranslation(m_position);
	//m.SetPositionVector(m_position);
	return m;
}

//Added by Sam for physics broadphase
void PhysicsNode::ConfigureAABBHalfLength()
{
	//Vector3 point = target->GetMesh()->GetFurthestVert();
	////scale the point
	//point * target->GetModelScale();

	////set the AABB half length
	//AABBHalfLength = length(point);
}