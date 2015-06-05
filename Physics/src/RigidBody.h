#ifndef _RIGIDBODY_H_
#define _RIGIDBODY_H_

#include "PhysicsObject.h"

namespace Physics
{

	class RigidBody : public PhysicsObject
	{
	public:
		RigidBody();
		RigidBody(vec2 a_position, vec2 a_velocity, float a_rotation, float a_mass);
		~RigidBody();

		virtual void update(vec2 a_gravity, float a_timeStep);

		virtual void debug();

		void applyForce(vec2 a_force);
		void applyForceToActor(RigidBody* a_other, vec2 a_force);

	public:
		vec2 m_position;
		vec2 m_velocity;

		float m_mass;
		float m_rotation;

	};

}

#endif // !_RIGIDBODY_H_
