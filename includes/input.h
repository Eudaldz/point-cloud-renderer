#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace Input{
	constexpr uint32_t KEY_MOVE_RIGHT = 0;
	constexpr uint32_t KEY_MOVE_LEFT = 1;
	constexpr uint32_t KEY_MOVE_FORWARD = 2;
	constexpr uint32_t KEY_MOVE_BACKWARD = 3;
	constexpr uint32_t KEY_MOVE_UP = 4;
	constexpr uint32_t KEY_MOVE_DOWN = 5;
	constexpr uint32_t KEY_ROTATE_LEFT = 6;
	constexpr uint32_t KEY_ROTATE_RIGHT = 7;

	constexpr uint32_t KEY_MODEL_TURN_ACTION = 8;
	constexpr uint32_t KEY_CAM_TURN_ACTION = 9;
	constexpr uint32_t KEY_CAM_RESET = 10;
	constexpr uint32_t KEY_MODEL_RESET = 11;

	constexpr uint32_t KEY_CYCLE_1 = 12;
	constexpr uint32_t KEY_CYCLE_2 = 13;
	constexpr uint32_t KEY_CYCLE_3 = 14;

	constexpr uint32_t KEY_MOD = 15;

	constexpr uint32_t KEY_TAB = 16;
	constexpr uint32_t KEY_Z = 17;
	constexpr uint32_t KEY_X = 18;
	constexpr uint32_t KEY_1 = 19;
	constexpr uint32_t KEY_2 = 20;
	constexpr uint32_t KEY_Y = 21;
	constexpr uint32_t KEY_ARROW_UP = 22;
	constexpr uint32_t KEY_ARROW_DOWN = 23;

	constexpr uint32_t HORIZONTAL_AXIS = 1; //a-d
	constexpr uint32_t VERTICAL_AXIS = 2; //space-shift
	constexpr uint32_t FRONTAL_AXIS = 3; //w-s
	constexpr uint32_t LATERAL_AXIS = 4; //e-q

	void Poll();
	bool GetButton(uint32_t button);
	bool GetButtonDown(uint32_t button);
	bool GetButtonUp(uint32_t button);
	bool ExitRequest();
	int GetKeyAxis(uint32_t axis);
	glm::vec2 GetMousePosition();
	glm::vec2 GetMouseDelta();
	float GetScrollDelta();
	
	void TrackWindow(GLFWwindow* window);
	void UntrackWindow();
}

