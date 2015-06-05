#ifndef _PHYSICSSCENE_H_
#define _PHYSICSSCENE_H_

#include "PhysicsObject.h"

#include <vector>

namespace Physics
{

	class PhysicsScene
	{
	public:
		PhysicsScene();
		~PhysicsScene();

		void addActor(PhysicsObject* a_actor);
		void removeActor(PhysicsObject* a_actor);

		void update();
		void debugScene();
		void addGizmos();

	public:
		vec2 m_gravity;

		float m_timeStep;

		std::vector<PhysicsObject*> m_actors;
	};

}

#endif // !_PHYSICSSCENE_H_
