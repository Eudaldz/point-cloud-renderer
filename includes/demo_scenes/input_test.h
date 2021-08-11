#pragma once
#include "scene.h"
#include "input.h"

class InputTest : public Scene
{
private:
	Input& in = Input::GetInstance();

public:
	void Start();
	void Update(float deltaTime);
	void Render();
	void End();
};