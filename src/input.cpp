#include "input.h"

#include <iostream>

Input Input::in;

namespace
{
	constexpr int N = 16;
	bool state[N] = { 0 };
	bool down[N] = { 0 };
	bool up[N] = { 0 };
	int buttons[N] = {
		GLFW_KEY_D, GLFW_KEY_A, GLFW_KEY_W, GLFW_KEY_S,
		GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT,
		GLFW_KEY_UP, GLFW_KEY_DOWN,
		GLFW_KEY_PERIOD, GLFW_KEY_COMMA,
		GLFW_KEY_P, GLFW_KEY_M,
		GLFW_MOUSE_BUTTON_RIGHT, GLFW_MOUSE_BUTTON_LEFT,
		GLFW_KEY_BACKSPACE, GLFW_KEY_LEFT_ALT
	};
	
	const int right_mouse_idx = 12;
	const int left_mouse_idx = 13;
	const int d_idx = 0;
	const int a_idx = 1;
	const int w_idx = 2;
	const int s_idx = 3;
	const int sp_idx = 4;
	const int sh_idx = 5;
	const int up_idx = 6;
	const int dn_idx = 7;

	double mouseX = 0, mouseY = 0;
	double mDeltaX = 0, mDeltaY = 0;
	double scrollDelta = 0;
	bool firstMouse = false;

	int get_button_index(int button)
	{
		int index = -1;
		for (int i = 0; i < N && index == -1; i++) {
			if (button == buttons[i]) {
				index = i;
			}
		}
		return index;
	}
	
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (action == GLFW_REPEAT)return;
		int index = get_button_index(key);
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
		if (button == GLFW_MOUSE_BUTTON_RIGHT) index = right_mouse_idx;
		else if (button == GLFW_MOUSE_BUTTON_LEFT) index = left_mouse_idx;
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

	void initialize()
	{
		for (int i = 0; i < N; i++) {
			state[i] = up[i] = down[i] = false;
		}
		mouseX = mouseY = scrollDelta = mDeltaX = mDeltaY = 0;
		firstMouse = true;
	}
	
	void reset_transitions()
	{
		for (int i = 0; i < N; i++) {
			down[i] = false;
			up[i] = false;
		}
		scrollDelta = 0;
		mDeltaX = 0;
		mDeltaY = 0;
	}
}

Input& Input::GetInstance()
{
	return Input::in;
}

void Input::TrackWindow(GLFWwindow* window)
{
	if (this->window != NULL) {
		throw "Attempted to track a window with another in progress.";
		return;
	}
	this->window = window;
	glfwSetKeyCallback(this->window, key_callback);
	glfwSetCursorPosCallback(this->window, cursor_position_callback);
	glfwSetMouseButtonCallback(this->window, mouse_button_callback);
	glfwSetScrollCallback(this->window, scroll_callback);
	initialize();
}

void Input::UntrackWindow()
{
	glfwSetKeyCallback(window, NULL);
	glfwSetCursorPosCallback(window, NULL);
	glfwSetMouseButtonCallback(window, NULL);
	glfwSetScrollCallback(window, NULL);
	this->window = NULL;
}

void Input::PollInput()
{
	reset_transitions();
	glfwPollEvents();
}

bool Input::GetButton(int button)
{
	int index = get_button_index(button);
	if (index != -1) {
		return state[index];
	}
	return false;
}

bool Input::ExitRequest()
{
	return glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
}

bool Input::GetButtonDown(int button)
{
	int index = get_button_index(button);
	if (index != -1) {
		return down[index];
	}
	return false;
}

bool Input::GetButtonUp(int button)
{
	int index = get_button_index(button);
	if (index != -1) {
		return up[index];
	}
	return false;
}

int Input::GetKeyAxis(int axis)
{
	int s = 0;
	if (axis == HORIZONTAL_AXIS) {
		s += state[d_idx];
		s -= state[a_idx];
	}
	else if (axis == VERTICAL_AXIS) {
		s += state[sp_idx];
		s -= state[sh_idx];
	}
	else if (axis == FRONTAL1_AXIS) {
		s += state[w_idx];
		s -= state[s_idx];
	}
	else if (axis == FRONTAL1_AXIS) {
		s += state[up_idx];
		s -= state[dn_idx];
	}
	return s;
}

vec2 Input::GetMousePosition()
{
	return vec2(mouseX, mouseY);
}

vec2 Input::GetMouseDelta()
{
	return vec2(mDeltaX, mDeltaY);
}

float Input::GetScrollDelta()
{
	return (float)scrollDelta;
}
