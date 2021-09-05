#include "camera_controller.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace glm;

constexpr int CameraController::MOVE_FREE = 0;
constexpr int CameraController::MOVE_PIVOT = 1;
constexpr float CameraController::MIN_LOOK_AT_DIST = 0.05f;
constexpr float CameraController::MIN_UP_THETA = glm::radians(5.0f);

constexpr vec3 CameraController::WORLD_UP = vec3(0,1,0);

CameraController::CameraController()
{
	camera = nullptr;
	defPosition = vec3(5, 5, 5);
	defLookAt = vec3(0, 0, 0);
	defViewSize = 1.0f;
	movementType = MOVE_PIVOT;
}

void CameraController::move(vec3 v)
{
	vec3 right = glm::normalize(glm::cross(camera->lookAt - camera->position, WORLD_UP));
	vec3 front = glm::normalize(glm::cross(WORLD_UP, right));
	vec3 up = glm::normalize(glm::cross(right, front));
	vec3 deltaPos = right * v.x + up * v.y + front * v.z;
	camera->position += deltaPos;
	camera->lookAt += deltaPos;
}

void CameraController::rotate(glm::vec2 v)
{
	vec3 camDir = glm::normalize(camera->lookAt - camera->position);
	vec3 right = glm::normalize(glm::cross(camDir, WORLD_UP));
	float horizontalDelta = v.x;
	float verticalDelta = -v.y;
	float t = glm::dot(camDir, WORLD_UP);
	float theta = glm::acos(t);
	constexpr float minTheta = MIN_UP_THETA;
	constexpr float maxTheta = glm::pi<float>() - minTheta;
	verticalDelta = glm::clamp(verticalDelta, minTheta - theta, maxTheta - theta);
	mat4 hRotation = glm::rotate(mat4(1.0f), horizontalDelta, WORLD_UP);
	mat4 vRotation = glm::rotate(mat4(1.0f), -verticalDelta, right);
	mat4 rotation = vRotation * hRotation;
	if (movementType == MOVE_PIVOT) {
		vec3 camVector = camera->position - camera->lookAt;
		camVector = rotation * vec4(camVector, 1.0f);
		camera->position = camera->lookAt + camVector;
	} else {
		camDir = rotation * vec4(camDir, 1.0f);
		camera->lookAt = camera->position + camDir;
	}
}

void CameraController::approach(float d)
{
	vec3 camDir = glm::normalize(camera->lookAt - camera->position);
	float dist = glm::length(camera->lookAt - camera->position);
	constexpr float minDist = MIN_LOOK_AT_DIST;
	d = glm::clamp(d, d, dist - minDist);
	camera->position += camDir * d;
}

void CameraController::zoom(float d)
{
	camera->viewSize *= (1-d);
}

void CameraController::resetToDefault()
{
	camera->position = defPosition;
	camera->lookAt = defLookAt;
	//camera->up = WORLD_UP;
	camera->viewSize = defViewSize;
}

void CameraController::changeMoveMode()
{
	if (movementType == MOVE_PIVOT) {
		movementType = MOVE_FREE;
	} else {
		movementType = MOVE_PIVOT;
	}
}

void CameraController::Start()
{
	if (camera == nullptr) {
		std::cout << "ERROR: Camera is null in CameraController\n";
		return;
	}
	defPosition = camera->position;
	defLookAt = camera->lookAt;
	defViewSize = camera->viewSize;
}

void CameraController::Update(float deltaTime)
{
	if (camera == nullptr) {
		std::cout << "ERROR: Camera is null in CameraController\n";
		return;
	}
	int horizontalAxis = in.GetKeyAxis(Input::HORIZONTAL_AXIS);
	int verticalAxis = in.GetKeyAxis(Input::VERTICAL_AXIS);
	int frontalAxis1 = in.GetKeyAxis(Input::FRONTAL1_AXIS);
	bool switchProj = in.GetButtonDown(Input::CHANGE_VIEW);
	bool switchMove = in.GetButtonDown(Input::CHANGE_MOVE);
	bool resetB = in.GetButtonDown(Input::CAM_RESET);
	bool camDrag = in.GetButton(Input::CAM_DRAG);
	bool modB = in.GetButton(Input::MOD);
	vec2 mouseDelta = in.GetMouseDelta();
	float scrollDelta = in.GetScrollDelta();
	
	bool isMove = horizontalAxis != 0 || verticalAxis != 0 || frontalAxis1 != 0;
	bool isApproach = frontalAxis1 != 0;

	vec3 deltaPos = vec3(horizontalAxis, verticalAxis, frontalAxis1) * moveSpeed * deltaTime;
	vec2 deltaRotate = glm::radians(mouseDelta * mouseCoef);
	float deltaApproach = frontalAxis1 * moveSpeed * deltaTime;
	float deltaZoom = scrollDelta * scrollCoef;

	if (switchProj) {
		camera->projType =
			camera->projType == Camera::ORTHO ?
			Camera::PERSP : Camera::ORTHO;
	}
	if (switchMove) changeMoveMode();
	if (resetB) resetToDefault();

	if(camDrag)rotate(deltaRotate);
	if(isMove && !modB)move(deltaPos);
	if(modB && isApproach && movementType == MOVE_PIVOT)approach(deltaApproach);
	if(scrollDelta != 0)zoom(deltaZoom);
}