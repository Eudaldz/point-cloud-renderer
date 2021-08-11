#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

Camera::Camera()
{
	position = vec3(0, 0, -2);
	lookAt = vec3(0, 0, 0);
	up = vec3(0, 1, 0);
	viewSize = 1.0f;
	aspectRatio = 4.0f / 3.0f;
	nearClip = 0.2f;
	farClip = 10.0f;
	focalLength = 1.0f;
	projType = ORTHO;
}

mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(this->position, this->lookAt, this->up);
}

mat4 Camera::GetOrthoProjMatrix()
{
	float height = this->viewSize;
	float width = height * aspectRatio;
	return glm::ortho(-width / 2.0f, width / 2.0f, -height / 2.0f, height / 2.0f, nearClip, farClip);
}

mat4 Camera::GetPerspProjMatrix()
{
	return glm::perspective(glm::atan(viewSize, 2.0f*focalLength)*2.0f, aspectRatio, nearClip, farClip);
}

mat4 Camera::GetProjMatrix()
{
	if (projType == ORTHO) {
		return GetOrthoProjMatrix();
	}
	return GetPerspProjMatrix();
}