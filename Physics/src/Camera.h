#ifndef _CAMERA_H_
#define _CAMERA_H_

#define GLM_SWIZZLE
#include "gl_core_4_4.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include <GLFW\glfw3.h>


using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

class Camera
{
public:
	Camera();
	Camera(float a_fov);
	~Camera();

	virtual void Update(float a_deltaTime) = 0;
	void setPerspective(float a_fov, float a_aspectRatio, float a_near, float a_far);
	void setLookAt(vec3 a_from, vec3 a_to, vec3 a_up);
	void setPosition(vec3 a_position);
	mat4 getWorldTransform();
	mat4 getView();
	mat4 getProjection();
	mat4 getProjectionView();

	vec4* getFrustrumPlanes(const mat4& a_transform);


protected:

	mat4 m_worldTransform;
	mat4 m_viewTransform;
	mat4 m_projectionTransform;
	mat4 m_projectionViewTransform;

	void updateProjectionViewTransform();

private:
	vec4 m_frustrum[6];

};

#endif // !_CAMERA_H_
