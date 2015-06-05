#include "myPhysicsEngine.h"

#include "gl_core_4_4.h"
#include "Gizmos.h"

#include "glm_header.h"

#include <GLFW\glfw3.h>
#include <cstdio>

#include "Sphere.h"


MyPhysicsEngine::MyPhysicsEngine(){}

MyPhysicsEngine::~MyPhysicsEngine(){}

bool MyPhysicsEngine::Startup()
{
	if (Application::Startup() == false)
		return false;

	m_backColor = vec4(0.0f,0.5f,0.0f,1.0f);

	glClearColor(m_backColor.r, m_backColor.g, m_backColor.b, m_backColor.a);

	m_ortho = glm::ortho<float>(-100, 100, -100/1.7f, 100/1.7f, -1, 1);

	Gizmos::create();

	//physics
	m_physicsScene = new Physics::PhysicsScene();
	m_physicsScene->m_gravity = vec2(0, 0);
	m_physicsScene->m_timeStep = 0.001f;

	//add balls to scene
	Physics::Sphere* newBall;
	newBall = new Physics::Sphere(vec2(-40,0), vec2(0, 0), 30.0f, 20, vec4(1, 0, 0, 1));
	m_physicsScene->addActor(newBall);

	//temp
	m_ball = newBall;

	//deltatime
	glfwSetTime(0.0);
	return true;
}

void MyPhysicsEngine::Shutdown()
{
	Gizmos::destroy();
}

bool MyPhysicsEngine::Update()
{
	//deltatime
	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	Gizmos::clear();

	if (Application::Update() == false)
		return false;

	if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		m_ball->applyForce(vec2(0,1));
	}
	if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		m_ball->applyForce(vec2(0, -1));
	}
	if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		m_ball->applyForce(vec2(-1, 0));
	}
	if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		m_ball->applyForce(vec2(1, 0));
	}

	//physics
	m_physicsScene->update();
	m_physicsScene->addGizmos();


	return true;
}

void MyPhysicsEngine::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT);

	//draw 2d actors
	Gizmos::draw2D(m_ortho);

	glfwSwapBuffers(this->m_window);
	glfwPollEvents();
}