#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

using namespace glm;

class Input
{
private:
	static Input in;
	
	GLFWwindow* window = NULL;
	Input() {}
public:
	Input(Input const&) = delete;
	void operator=(Input const&) = delete;
	
	static const int MOVE_RIGHT = GLFW_KEY_D;
	static const int MOVE_LEFT = GLFW_KEY_A;
	static const int MOVE_FORWARD = GLFW_KEY_W;
	static const int MOVE_BACKWARD = GLFW_KEY_S;
	static const int MOVE_UP = GLFW_KEY_SPACE;
	static const int MOVE_DOWN = GLFW_KEY_LEFT_SHIFT;
	static const int MOVE_IN = GLFW_KEY_UP;
	static const int MOVE_OUT = GLFW_KEY_DOWN;
	static const int SPEED_UP = GLFW_KEY_PERIOD;
	static const int SPEED_DOWN = GLFW_KEY_COMMA;
	static const int CHANGE_VIEW = GLFW_KEY_P;
	static const int CHANGE_MOVE = GLFW_KEY_M;
	static const int CAM_DRAG = GLFW_MOUSE_BUTTON_LEFT;
	static const int CAM_RESET = GLFW_KEY_BACKSPACE;
	static const int MOD = GLFW_KEY_LEFT_ALT;

	static const int HORIZONTAL_AXIS = 1;
	static const int VERTICAL_AXIS = 2;
	static const int FRONTAL1_AXIS = 3;
	static const int FRONTAL2_AXIS = 4;

	static Input& GetInstance();
	
	void TrackWindow(GLFWwindow* window);
	void UntrackWindow();
	void PollInput();

	bool GetButton(int button);
	bool GetButtonDown(int button);
	bool GetButtonUp(int button);
	bool ExitRequest();
	int GetKeyAxis(int axis);
	vec2 GetMousePosition();
	vec2 GetMouseDelta();
	float GetScrollDelta();
};