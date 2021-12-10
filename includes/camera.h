#pragma once
#include <glm/glm.hpp>

class Camera
{
public:
	glm::vec3 position;
	glm::vec3 lookAt;
	glm::vec3 up;
	float viewSize;
	float aspectRatio;
	float nearClip;
	float farClip;
	float focalLength;
	int projType;

	static const int ORTHO = 0;
	static const int PERSP = 1;

	Camera();
	glm::mat4 GetViewMatrix();
	glm::mat4 GetWorldMatrix();
	glm::mat4 GetViewDirMatrix();
	glm::mat4 GetPerspProjMatrix();
	glm::mat4 GetOrthoProjMatrix();
	glm::mat4 GetProjMatrix();
};