#include "Sphere.h"

#include "Gizmos.h"

using Physics::Sphere;

Sphere::Sphere()
{

}

Sphere::Sphere(vec2 a_position, vec2 a_velocity, float a_mass, float a_radius, vec4 a_color) : RigidBody(a_position,a_velocity, 0.0f, a_mass)
{
	m_radius = a_radius;
	m_color = a_color;
}

Sphere::~Sphere()
{

}

void Sphere::makeGizmo()
{
	Gizmos::add2DCircle(m_position, m_radius, 30, m_color);
}