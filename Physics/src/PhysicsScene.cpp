#include "PhysicsScene.h"

using Physics::PhysicsScene;
using Physics::PhysicsObject;

PhysicsScene::PhysicsScene()
{

}

PhysicsScene::~PhysicsScene()
{

}

void PhysicsScene::addActor(PhysicsObject* a_actor)
{
	m_actors.push_back(a_actor);
}

void PhysicsScene::removeActor(PhysicsObject* a_actor)
{
	for (unsigned int i = 0; i < m_actors.size(); i++)
	{
		if (m_actors[i] == a_actor)
		{
			delete a_actor;
			m_actors.erase(m_actors.begin() + i);
		}
	}
}

void PhysicsScene::update()
{
	for (unsigned int i = 0; i < m_actors.size(); i++)
	{
		m_actors[i]->update(m_gravity, m_timeStep);
	}
}

void PhysicsScene::debugScene()
{

}

void PhysicsScene::addGizmos()
{
	for (unsigned int i = 0; i < m_actors.size(); i++)
	{
		m_actors[i]->makeGizmo();
	}
}