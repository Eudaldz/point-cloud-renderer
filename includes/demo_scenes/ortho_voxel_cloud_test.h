#pragma once
#include "scene.h"
#include "camera.h"
#include "background.h"
#include "camera_controller.h"

class OrthoVoxelCloudTest : public Scene
{
private:
	static constexpr size_t SAMPLES = 16u;
	static constexpr unsigned int SAMPLE_RES = 32;
	
	static constexpr float SQ_WIDTH = 5.0f;
	static constexpr float SQ_HEIGHT = 5.0f;
	
	float *voxels;
    unsigned char footprint[SAMPLE_RES*SAMPLE_RES];
	float voxelSize;

	Camera camera;
	Background background;
	CameraController cont;

	unsigned int VBO, VAO;
	unsigned int programId;
	unsigned int textureId;

	void generateVoxels();
	void loadModelData();

public:
	OrthoVoxelCloudTest();
	void Start();
	void Update(float deltaTime);
	void Render();
	void End();
};