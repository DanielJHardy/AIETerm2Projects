#include "Application.h"
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include <cstdio>


Application::Application(){}
Application::~Application(){}

bool Application::Startup()
{
	// Loads up glfw
	if (glfwInit() == 0)
		return false;

	//creates a window
	m_window = glfwCreateWindow(1280, 720, "OpenGl Project", nullptr, nullptr);

	// CHeck for window creation failure
	if (m_window == nullptr)
		return false;

	//does something
	glfwMakeContextCurrent(m_window);


	//load opengl
	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
		return false;
	}

	int major_version = ogl_GetMajorVersion();
	int minor_version = ogl_GetMinorVersion();

	printf("Successfully loaded OpenGL version %d.%d\n", major_version, minor_version);

	return true;
}

void Application::Shutdown()
{
	glfwDestroyWindow(this->m_window);
	glfwTerminate();
}

bool Application::Update()
{
	if (glfwWindowShouldClose(this->m_window))
		return false;


	return true;

}

void Application::Draw()
{

}