#include "demo_scenes/input_test.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>


void InputTest::Start()
{
	
}

void InputTest::Update(float deltaTime)
{
	int buttons[12] = { 
		Input::MOVE_LEFT, Input::MOVE_RIGHT, 
		Input::MOVE_FORWARD, Input::MOVE_BACKWARD,
		Input::MOVE_UP, Input::MOVE_DOWN,
		Input::MOVE_IN, Input::MOVE_OUT,
		Input::SPEED_UP, Input::SPEED_DOWN,
		Input::CHANGE_MOVE, Input::CHANGE_VIEW
	};

	const char* labels[12] = {
		"MOVE LEFT", "MOVE RIGHT",
		"MOVE FORWARD", "MOVE BACKWARD",
		"MOVE UP", "MOVE DOWN",
		"MOVE IN", "MOVE OUT",
		"SPEED UP", "SPEED DOWN",
		"CHANGE MOVE MODE", "CHANGE VIEW MODE"
	};
	bool action = false;
	for (int i = 0; i < 12; i++) {
		if (in.GetButtonDown(buttons[i])) {
			action = true;
			std::cout << "+" << labels[i] << " | ";
		}
		if (in.GetButtonUp(buttons[i])) {
			action = true;
			std::cout << "-" << labels[i] << " | ";
		}
	}
	
	if (action) {
		std::cout << std::endl;
	}

	float sd = in.GetScrollDelta();
	if (sd != 0) {
		std::cout << "scroll :: " << sd << std::endl;
	}

	if (in.GetButton(Input::CAM_DRAG)) {
		glm::vec2 mp = in.GetMousePosition();
		std::cout << mp.x << ", " << mp.y << std::endl;
	}
}

void InputTest::Render()
{

}

void InputTest::End()
{

}