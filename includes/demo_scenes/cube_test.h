#pragma once
#include "scene.h"
#include "camera.h"
#include <glm/glm.hpp>

class CubeTest : public Scene 
{
private:
	static const char* vShaderCode;
	static const char* fShaderCode;
    static const float vertices[192];
	static const unsigned int indices[36];

	static const float CAM_ROT_SPEED;
	static const float EDGE_COLOR[3];
	static const float EDGE_OFFSET;
	static const float BACKGROUND_COLOR[3];

    unsigned int VBO, VAO, EBO;
    unsigned int programId;
	Camera camera;

    void loadModelData();

public:
	CubeTest();
	void Start();
	void Update(float deltaTime);
	void Render();
	void End();
};
