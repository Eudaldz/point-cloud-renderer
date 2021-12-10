#pragma once
#include <glm/glm.hpp>
#include "camera.h"

class ViewController
{
private:
	//static const float MIN_LOOK_AT_DIST;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;

	void move(glm::vec3 v);
	void turn(glm::vec2 v, bool world);
	void rotate(float d);
	void approach(float d);
	void zoom(float d);
	void resetCamera();
	void resetModel();

	void turnModel(glm::vec2, bool world);
	void rotateModel(float d);
	void resizeModel(float d);

	Camera& camera;
	glm::mat4& modelT;
	float& modelSize;

public:
	
	glm::vec3 defaultPosition;
	glm::vec3 defaultLookAt;
	glm::vec3 defaultUp;
	float defaultViewSize;
	glm::mat4 defaultModelT;
	float defaultModelSize;

	float rotateSpeed = 1;
	float moveSpeed = 1;
	float turnSpeed = 2.0f;
	float mouseCoef = -1;
	float scrollCoef = 0.25f;
	float resizeCoef = 0.1f;

	ViewController(Camera& camera, glm::mat4& modelT, float& modelSize);
	void SetCurrentAsDefault();
	void Update(float deltaTime);
};
