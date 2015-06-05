#include "RigidBody.h"

using Physics::RigidBody;

RigidBody::RigidBody(){}

RigidBody::RigidBody(vec2 a_position, vec2 a_velocity, float a_rotation, float a_mass)
{
	m_position = a_position;
	m_velocity = a_velocity;
	m_rotation = a_rotation;
	m_mass = a_mass;
}

RigidBody::~RigidBody(){}

void RigidBody::update(vec2 a_gravity, float a_timeStep)
{
	m_velocity += a_gravity * a_timeStep;
	m_position += m_velocity;
}

void RigidBody::debug()
{

}

void RigidBody::applyForce(vec2 a_force)
{
	m_velocity += a_force / m_mass;
}

void RigidBody::applyForceToActor(RigidBody* a_other, vec2 a_force)
{

}