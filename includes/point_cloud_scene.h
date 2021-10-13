#pragma once
#include "scene.h"
#include "point_cloud.h"
#include "shader_program.h"
#include "camera.h"
#include "background.h"
#include "camera_controller.h"

class PointCloudScene : public Scene
{
private:
	ShaderProgram* shader;
	PointCloud* pointcloud;

	Camera camera;
	Background background;
	CameraController cont;
	float pointSizeTransform;

	void normalizePointCloud();

public: 
	PointCloudScene(ShaderProgram* sp, PointCloud* pc);
	void Start();
	void Update(float deltaTime);
	void Render();
	void End();
};
