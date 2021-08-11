#pragma once
#include "scene.h"
#include "camera.h"
#include "background.h"
#include "camera_controller.h"
#include <glm/glm.hpp>

class CameraTest : public Scene
{
private:
	static const char* vShaderCode;
	static const char* fShaderCode;
	static const float vertices[120];
	static const unsigned int indices[36];
	static const glm::vec3 cubePositions[5];
	static const glm::vec3 cubeAxis[5];

	static const float CUBE_COLORS[3*5];
	static const float EDGE_COLOR[3];
	static const float EDGE_OFFSET;
	static const glm::vec3 TOP_BG_COLOR;
	static const glm::vec3 BOTTOM_BG_COLOR;

	unsigned int VBO, VAO, EBO;
	unsigned int programId;
	Camera camera;
	CameraController cont;
	Background background;
	float rotation;

	void loadModelData();


public:
	CameraTest();
	void Start();
	void Update(float deltaTime);
	void Render();
	void End();
};


