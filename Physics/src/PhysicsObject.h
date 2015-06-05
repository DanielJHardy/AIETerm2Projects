#ifndef _PHYSICSOBJECT_H_
#define _PHYSICSOBJECT_H_

#include "glm_header.h"

namespace Physics
{

	enum ShapeType
	{
		PLANE =		0,
		SPHERE =	1,
		BOX =		2,
	};

	class PhysicsObject
	{
	public:
		ShapeType m_shapeID;

	public:
		virtual void update(vec2 a_gravity, float a_timeStep) = 0;
		virtual void debug() = 0;
		virtual void makeGizmo() = 0;
		virtual void resetPosition(){};


	};

}

#endif // !_PHYSICSOBJECT_H_
