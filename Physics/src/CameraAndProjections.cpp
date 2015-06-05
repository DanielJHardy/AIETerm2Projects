#include "CameraAndProjections.h"

#include "gl_core_4_4.h"
#include "Gizmos.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include <GLFW\glfw3.h>

#include <cstdio>

CameraAndProjections::CameraAndProjections()
{

}
CameraAndProjections::~CameraAndProjections()
{
}

bool CameraAndProjections::Startup()
{
	if (Application::Startup() == false)
	{
		return false;
	}

	m_backColor = vec4(0.3f,0.3f,0.3f,1.0f);

	glClearColor(m_backColor.r, m_backColor.g, m_backColor.b, m_backColor.a);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	//Camera
	m_sceneCam = FlyCamera(glm::radians(60.0f));
	m_sceneCam.setLookAt(vec3(-1, 0, -1), vec3(0,0,0), vec3(0, 1, 0));

	//GUI
	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);

	m_bar = TwNewBar("Awesome Bar");
	TwAddVarRW(m_bar, "Clear Color", TW_TYPE_COLOR4F, &m_backColor, "");
	TwAddSeparator(m_bar, "sep1", "");
	TwAddVarRO(m_bar, "FPS", TW_TYPE_FLOAT, &m_fps, "");

	//
	glfwSetTime(0.0);
	return true;
}
void CameraAndProjections::Shutdown()
{
	TwDeleteAllBars();
	TwTerminate();

	Gizmos::destroy();
}

bool CameraAndProjections::Update()
{
	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	m_fps = 1 / dt;

	glClearColor(m_backColor.r, m_backColor.g, m_backColor.b, m_backColor.a);

	Gizmos::clear();

	if (Application::Update() == false)
	{
		return false;
	}

	Gizmos::add2DCircle(glm::vec2(100, 100), 100, 50, vec4(1, 0, 0, 1));

	m_sceneCam.Update(dt);

	vec4 white(1);
	vec4 black(0.1f, 0.1f, 0.1f, 1);

	for (int i = 0; i <= 20; i++)
	{
		Gizmos::addLine(vec3(-10 + i, -1, -10), vec3(-10 + i, -1, 10), i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, -1, -10 + i), vec3(10, -1, -10 + i), i == 10 ? white : black);
	}
	Gizmos::addTransform(mat4(1), 1);

	return true;
}
void CameraAndProjections::Draw()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	Gizmos::draw(m_sceneCam.getProjectionView());
	Gizmos::draw2D(glm::ortho(0.0f,1280.0f,720.0f,0.0f));
	TwDraw();

	glfwSwapBuffers(this->m_window);
	glfwPollEvents();


}