#include "input.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace {
	GLFWwindow* window = nullptr;
	
	constexpr uint32_t N = 24;
	constexpr int buttons[N] = {
		GLFW_KEY_D, GLFW_KEY_A, GLFW_KEY_W, GLFW_KEY_S,
		GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT,
		GLFW_KEY_Q, GLFW_KEY_E,
		GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_RIGHT,
		GLFW_KEY_R, GLFW_KEY_T,
		GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3,
		GLFW_KEY_LEFT_CONTROL, 
		GLFW_KEY_TAB,
		GLFW_KEY_Z,
		GLFW_KEY_X,
		GLFW_KEY_1,
		GLFW_KEY_2,
		GLFW_KEY_Y,
		GLFW_KEY_UP, GLFW_KEY_DOWN
	};

	bool state[N] = { 0 };
	bool down[N] = { 0 };
	bool up[N] = { 0 };
	double mouseX = 0, mouseY = 0;
	double mDeltaX = 0, mDeltaY = 0;
	double scrollDelta = 0;
	bool firstMouse = false;

	void initialize() {
		for (int i = 0; i < N; i++) {
			state[i] = up[i] = down[i] = false;
		}
		mouseX = mouseY = scrollDelta = mDeltaX = mDeltaY = 0;
		firstMouse = true;
	}

	void resetTransitions() {
		for (int i = 0; i < N; i++) {
			down[i] = false;
			up[i] = false;
		}
		scrollDelta = 0;
		mDeltaX = 0;
		mDeltaY = 0;
	}

	int getCodeIndex(int glfw_value)
	{
		for (int i = 0; i < N; i++) {
			if (glfw_value == buttons[i]) {
				return i;
			}
		}
		return -1;
	}

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (action == GLFW_REPEAT)return;
		int index = getCodeIndex(key);
		if (index != -1) {
			if (action == GLFW_PRESS) {
				down[index] = true;
				state[index] = true;
			}
			else {
				up[index] = true;
				state[index] = false;
			}
		}
	}

	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		if (firstMouse) {
			mouseX = xpos;
			mouseY = ypos;
			firstMouse = false;
		}
		mDeltaX += xpos - mouseX;
		mDeltaY += ypos - mouseY;
		mouseX = xpos;
		mouseY = ypos;
	}

	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
		if (action == GLFW_REPEAT)return;
		int index = -1;
		if (button == GLFW_MOUSE_BUTTON_RIGHT) index = Input::KEY_CAM_TURN_ACTION;
		else if (button == GLFW_MOUSE_BUTTON_LEFT) index = Input::KEY_MODEL_TURN_ACTION;
		if (index != -1) {
			if (action == GLFW_PRESS) {
				down[index] = true;
				state[index] = true;
			}
			else {
				up[index] = true;
				state[index] = false;
			}
		}
	}

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		scrollDelta += yoffset;
	}
}


void Input::Poll()
{
	resetTransitions();
	glfwPollEvents();
}

bool Input::GetButton(uint32_t button)
{
	if (button < N) {
		return state[button];
	}
	return false;
}

bool Input::GetButtonDown(uint32_t button)
{
	if (button < N) {
		return down[button];
	}
	return false;
}

bool Input::GetButtonUp(uint32_t button)
{
	if (button < N) {
		return up[button];
	}
	return false;
}

bool Input::ExitRequest()
{
	return glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
}

int Input::GetKeyAxis(uint32_t axis)
{
	int s = 0;
	if (axis == Input::HORIZONTAL_AXIS) {
		s += state[Input::KEY_MOVE_RIGHT];
		s -= state[Input::KEY_MOVE_LEFT];
	}
	else if (axis == Input::VERTICAL_AXIS) {
		s += state[Input::KEY_MOVE_UP];
		s -= state[Input::KEY_MOVE_DOWN];
	}
	else if (axis == Input::FRONTAL_AXIS) {
		s += state[Input::KEY_MOVE_FORWARD];
		s -= state[Input::KEY_MOVE_BACKWARD];
	}
	else if (axis == Input::LATERAL_AXIS) {
		s += state[Input::KEY_ROTATE_RIGHT];
		s -= state[Input::KEY_ROTATE_LEFT];
	}
	return s;
}

glm::vec2 Input::GetMousePosition()
{
	return glm::vec2(mouseX, mouseY);
}

glm::vec2 Input::GetMouseDelta()
{
	return glm::vec2(mDeltaX, mDeltaY);
}

float Input::GetScrollDelta() 
{
	return (float)scrollDelta;
}

void Input::TrackWindow(GLFWwindow* _window)
{
	if (window != nullptr) {
		throw "Attempted to track a window with another in progress.";
		return;
	}
	window = _window;
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	initialize();
}

void Input::UntrackWindow()
{
	glfwSetKeyCallback(window, nullptr);
	glfwSetCursorPosCallback(window, nullptr);
	glfwSetMouseButtonCallback(window, nullptr);
	glfwSetScrollCallback(window, nullptr);
	window = nullptr;
}