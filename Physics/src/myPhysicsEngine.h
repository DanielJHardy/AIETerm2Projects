#ifndef _MYPHYSICSENGINE_H_
#define _MYPHYSICSENGINE_H_

#include "Application.h"

#include "PhysicsScene.h"
#include "Sphere.h"

class MyPhysicsEngine : public Application
{
public:
	MyPhysicsEngine();
	virtual ~MyPhysicsEngine();

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

private:

	vec4 m_backColor;

	mat4 m_ortho;

	Physics::PhysicsScene* m_physicsScene;

	Physics::Sphere* m_ball;

};

#endif // !_MYPHYSICSENGINE_H_
