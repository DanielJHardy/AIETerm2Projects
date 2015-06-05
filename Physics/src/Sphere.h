#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "RigidBody.h"

namespace Physics
{
	class Sphere: public RigidBody
	{
	public:
		Sphere();
		Sphere(vec2 a_position, vec2 a_velocity, float a_mass, float a_radius, vec4 a_color);
		~Sphere();

		virtual void makeGizmo();

	public:
		float m_radius;
		vec4 m_color;

	};
}

#endif // !_SPHERE_H_
