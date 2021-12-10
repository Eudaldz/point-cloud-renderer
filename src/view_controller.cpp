#include "view_controller.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "input.h"

using namespace glm;

ViewController::ViewController(Camera& camera, mat4& modelT, float& modelSize):camera(camera), modelT(modelT), modelSize(modelSize)
{
	SetCurrentAsDefault();
}

void ViewController::SetCurrentAsDefault()
{
	
	defaultPosition = camera.position;
	defaultLookAt = camera.lookAt;
	vec3 front = defaultLookAt - defaultPosition;
	vec3 up = glm::normalize(glm::cross(glm::cross(front, camera.up), front));
	defaultUp = up;
	camera.up = up;
	defaultViewSize = camera.viewSize;
	
	defaultModelT = modelT;
	defaultModelSize = modelSize;
}

void ViewController::Update(float deltaTime) 
{
	front = glm::normalize(camera.lookAt - camera.position);
	up = camera.up;
	right = glm::cross(front, up);
	
	bool camReset = Input::GetButtonDown(Input::KEY_CAM_RESET);
	bool modelReset = Input::GetButtonDown(Input::KEY_MODEL_RESET);

	bool cameraTurn = Input::GetButton(Input::KEY_CAM_TURN_ACTION);
	bool modelTurn = Input::GetButton(Input::KEY_MODEL_TURN_ACTION);
	bool modifier = Input::GetButton(Input::KEY_MOD);
	
	int horizontalAxis = Input::GetKeyAxis(Input::HORIZONTAL_AXIS);
	int verticalAxis = Input::GetKeyAxis(Input::VERTICAL_AXIS);
	int frontalAxis = Input::GetKeyAxis(Input::FRONTAL_AXIS);

	if (camReset) {
		resetCamera();
	}
	if (modelReset) {
		resetModel();
	}

	if (cameraTurn) {
		//MOVE
		vec3 deltaPos = vec3(horizontalAxis, verticalAxis, frontalAxis) * moveSpeed * deltaTime;
		if (deltaPos != vec3(0, 0, 0))move(deltaPos);
		//TURN
		vec2 deltaTurn = Input::GetMouseDelta() / 100.0f * mouseCoef;
		if (deltaTurn != vec2(0, 0))turn(deltaTurn, false);
		//ROTATE
		float deltaRotate = Input::GetKeyAxis(Input::LATERAL_AXIS) * rotateSpeed * deltaTime;
		if (deltaRotate != 0)rotate(deltaRotate);
	}
	else if (modelTurn) {
		//TURN
		vec2 deltaTurn1 = -Input::GetMouseDelta() / 100.0f * mouseCoef;
		vec2 deltaTurn2 = vec2(horizontalAxis, frontalAxis) * turnSpeed * deltaTime;
		vec2 deltaTurn = deltaTurn1 + deltaTurn2;
		if (deltaTurn != vec2(0, 0))turnModel(deltaTurn, false);
		float deltaRotate = Input::GetKeyAxis(Input::LATERAL_AXIS) * rotateSpeed * deltaTime;
		if (deltaRotate != 0)rotateModel(deltaRotate);

	}
	else {
		//TURN
		vec2 deltaTurn = vec2(horizontalAxis, -frontalAxis) * turnSpeed * deltaTime;
		if (deltaTurn != vec2(0, 0))turn(deltaTurn, true);
		//APPROACH
		float deltaApproach = verticalAxis * moveSpeed * deltaTime;
		if (deltaApproach != 0.0f) approach(deltaApproach);
		//ROTATE
		float deltaRotate = Input::GetKeyAxis(Input::LATERAL_AXIS) * rotateSpeed * deltaTime;
		if (deltaRotate != 0)rotate(deltaRotate);
	}
	if (!modifier) {
		float deltaZoom = Input::GetScrollDelta() * scrollCoef;
		if (deltaZoom != 0)zoom(deltaZoom);
	}
	else {
		float deltaResize = Input::GetScrollDelta() * resizeCoef;
		if (deltaResize != 0)resizeModel(deltaResize);
	}
	
}

void ViewController::move(glm::vec3 v) 
{
	vec3 deltaPos = right * v.x + up * v.y + front * v.z;
	camera.position += deltaPos;
	camera.lookAt += deltaPos;
}

void ViewController::turn(glm::vec2 v, bool world)
{
	float d = glm::length(camera.lookAt - camera.position);

	float horizontalDelta = v.x;
	float verticalDelta = v.y;
	vec3 hRotAxis = up;
	vec3 vRotAxis = right;
	if (world) {
		hRotAxis = vec3(0, 1, 0);
	}
	mat4 hRotation = glm::rotate(mat4(1.0f), horizontalDelta, hRotAxis);
	mat4 vRotation = glm::rotate(mat4(1.0f), verticalDelta, vRotAxis);
	mat4 rotation = vRotation * hRotation;

	camera.position = camera.lookAt + d*(vec3)(rotation*vec4(-front, 0));
	camera.up = glm::normalize(rotation * vec4(up, 0));
}

void ViewController::rotate(float d)
{
	mat4 rotation = glm::rotate(mat4(1.0f), d, front);
	camera.up = glm::normalize(rotation * vec4(camera.up, 0));
}

void ViewController::approach(float d)
{
	vec3 v = camera.lookAt - camera.position;
	float dist = glm::length(v);
	if (dist - d > 0.005f) {
		camera.position += glm::normalize(v) * d;
	}
}

void ViewController::zoom(float d)
{
	camera.viewSize *= (1 - d);
}

void ViewController::resetCamera()
{
	camera.position = defaultPosition;
	camera.lookAt = defaultLookAt;
	camera.up = defaultUp;
	camera.viewSize = defaultViewSize;
}

void ViewController::resetModel()
{
	modelT = defaultModelT;
	modelSize = defaultModelSize;
}

void ViewController::turnModel(glm::vec2 v, bool world)
{
	if (world) {
		mat4 hRotation = glm::rotate(mat4(1.0f), v.x, vec3(0,1,0));
		mat4 vRotation = glm::rotate(mat4(1.0f), v.y, vec3(1, 0, 0));
		modelT *= vRotation * hRotation;
	}
	else {
		mat4 hRotation = glm::rotate(mat4(1.0f), v.x, up);
		mat4 vRotation = glm::rotate(mat4(1.0f), v.y, right);
		modelT *= vRotation * hRotation;
	}
}

void ViewController::rotateModel(float d)
{
	mat4 rotation = glm::rotate(mat4(1.0f), d, front);
	modelT *= rotation;
}

void ViewController::resizeModel(float d)
{
	modelSize *= (1 - d);
}