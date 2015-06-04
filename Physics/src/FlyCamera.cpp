#include "FlyCamera.h"

const double centerX = 1280 / 2;
const double centerY = 720 / 2;

FlyCamera::FlyCamera(){}
FlyCamera::FlyCamera(float a_fov) : Camera(a_fov)
{
	m_speed = 30;
	m_up = vec3(0, 1, 0);
	m_mouseSensitivity = 1.f;

	m_yaw = 0;
	m_pitch = 0;
}

FlyCamera::~FlyCamera(){}

void FlyCamera::Update(float a_deltaTime)
{
	float mod = m_speed;

	//check for boost
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		mod *= 2;
	else if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		mod /= 4;


	//Forward
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS)
	{
		m_worldTransform[3] -= m_worldTransform[2] * mod * a_deltaTime;
	}
	//Backward
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS)
	{
		m_worldTransform[3] += m_worldTransform[2] * mod * a_deltaTime;
	}

	//Left
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A) == GLFW_PRESS)
	{
		m_worldTransform[3] -= m_worldTransform[0] * mod * a_deltaTime;
	}
	//Right
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D) == GLFW_PRESS)
	{
		m_worldTransform[3] += m_worldTransform[0] * mod * a_deltaTime;
	}

	//Up
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_Q) == GLFW_PRESS)
	{
		m_worldTransform[3] -= m_worldTransform[1] * mod * a_deltaTime;
	}
	//Down
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_E) == GLFW_PRESS)
	{
		m_worldTransform[3] += m_worldTransform[1] * mod * a_deltaTime;
	}

	//Rotation
	if (glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		double x, y;

		//Get and reset the mouse position
		glfwGetCursorPos(glfwGetCurrentContext(), &x, &y);
		glfwSetCursorPos(glfwGetCurrentContext(), centerX, centerY);

		x -= centerX;
		y -= centerY;

		x /= centerX;
		y /= centerY;

		x *= -m_mouseSensitivity;
		y *= -m_mouseSensitivity;

		m_yaw += (float)x;
		m_pitch += (float)y;

		// restrict pitch
		if (m_pitch > glm::radians(90.f))
			m_pitch = glm::radians(90.f);
		else if (m_pitch < glm::radians(-90.f))
			m_pitch = glm::radians(-90.f);

		//Do the rotating
		mat4 p_mat = glm::rotate(m_pitch, vec3(1,0,0));
		mat4 y_mat = glm::rotate(m_yaw, vec3(0,1,0));

		mat4 transform = y_mat * p_mat;

		transform[3] = m_worldTransform[3];
		m_worldTransform = transform;

	}



	m_viewTransform = glm::inverse(m_worldTransform);
	updateProjectionViewTransform();
}

void FlyCamera::setSpeed(float a_speed)
{
	m_speed = a_speed;
}