#pragma once
#include "scene.h"
#include "camera.h"
#include <glm/glm.hpp>

class Background : public Scene
{
private:
	static const char* vShaderCode;
	static const char* fShaderCode;
	static const float vertices[12];
	static const unsigned int indices[6];
	
	unsigned int VBO, VAO, EBO;
	unsigned int programId;

	void loadModelData();

public:
	Camera* camera;
	glm::vec3 topColor;
	glm::vec3 bottomColor;

	Background();
	void Start();
	void Update(float deltaTime);
	void Render();
	void End();
};