#pragma once
#include <glm/glm.hpp>
#include "camera.h"
#include "input.h"

class CameraController
{
private:
	static const int MOVE_FREE;
	static const int MOVE_PIVOT;

	static const glm::vec3 WORLD_UP;
	static const float MIN_LOOK_AT_DIST;
	static const float MIN_UP_THETA;
	
	int movementType;

	glm::vec3 defPosition;
	glm::vec3 defLookAt;
	float defViewSize;

	Input& in = Input::GetInstance();

	void move(glm::vec3 v);
	void rotate(glm::vec2 v);
	void approach(float d);
	void zoom(float d);
	void resetToDefault();
	void changeMoveMode();

public:
	Camera* camera;

	float moveSpeed = 1;
	float mouseCoef = -1;
	float scrollCoef = 1;

	CameraController();
	void Start();
	void Update(float deltaTime);
};
