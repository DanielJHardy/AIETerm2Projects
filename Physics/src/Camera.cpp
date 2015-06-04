#include "Camera.h"

Camera::Camera(){}
Camera::Camera(float a_fov)
{
	m_viewTransform = glm::lookAt(vec3(-1, 2, -1), vec3(0, 0, 0), vec3(0, 1, 0));
	m_worldTransform = glm::inverse(m_viewTransform);

	m_projectionTransform = glm::perspective(a_fov, 1280.0f / 720.0f, 0.1f, 10000.0f);
	m_projectionViewTransform = m_projectionTransform * m_viewTransform;
}

Camera::~Camera(){}

void Camera::setPerspective(float a_fov, float a_aspectRatio, float a_near, float a_far)
{
	m_projectionTransform = glm::perspective(a_fov, a_aspectRatio, a_near, a_far);
	updateProjectionViewTransform();
}

void Camera::setLookAt(vec3 a_from, vec3 a_to, vec3 a_up)
{
	m_viewTransform = glm::lookAt(a_from, a_to, a_up);
	m_worldTransform = glm::inverse(m_viewTransform);
	updateProjectionViewTransform();
}

void Camera::setPosition(vec3 a_position)
{
	m_worldTransform[3].xyz = a_position;
	m_viewTransform = glm::inverse(m_worldTransform);
	updateProjectionViewTransform();
}

mat4 Camera::getWorldTransform()
{
	return m_worldTransform;
}

mat4 Camera::getView()
{
	return m_viewTransform;
}

mat4 Camera::getProjection()
{
	return m_projectionTransform;
}

mat4 Camera::getProjectionView()
{
	return m_projectionViewTransform;
}

void Camera::updateProjectionViewTransform()
{
	m_projectionViewTransform = m_projectionTransform * m_viewTransform;
}

vec4* Camera::getFrustrumPlanes(const mat4& a_transform)
{

	//right side
	m_frustrum[0] = vec4(	a_transform[0][3] - a_transform[0][0], //is the 1 a typo?
							a_transform[1][3] - a_transform[1][0],
							a_transform[2][3] - a_transform[2][0],
							a_transform[3][3] - a_transform[3][0]);
	//left side
	m_frustrum[1] = vec4(	a_transform[0][3] + a_transform[0][0],
							a_transform[1][3] + a_transform[1][0],
							a_transform[2][3] + a_transform[2][0],
							a_transform[3][3] + a_transform[3][0]);
	//top
	m_frustrum[2] = vec4(	a_transform[0][3] - a_transform[0][1],
							a_transform[1][3] - a_transform[1][1],
							a_transform[2][3] - a_transform[2][1],
							a_transform[3][3] - a_transform[3][1]);
	//bottom
	m_frustrum[3] = vec4(	a_transform[0][3] + a_transform[1][1],
							a_transform[1][3] + a_transform[1][1],
							a_transform[2][3] + a_transform[2][1],
							a_transform[3][3] + a_transform[3][1]);
	//far
	m_frustrum[4] = vec4(	a_transform[0][3] - a_transform[0][2],
							a_transform[1][3] - a_transform[1][2],
							a_transform[2][3] - a_transform[2][2],
							a_transform[3][3] - a_transform[3][2]);
	//near													   
	m_frustrum[5] = vec4(	a_transform[0][3] + a_transform[1][2],
							a_transform[1][3] + a_transform[1][2],
							a_transform[2][3] + a_transform[2][2],
							a_transform[3][3] + a_transform[3][2]);

	for (int i = 0; i < 6; i++)
		m_frustrum[i] = glm::normalize(m_frustrum[i]);

	return m_frustrum;
}