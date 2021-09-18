#include "point_cloud_scene.h"

PointCloudScene::PointCloudScene(ShaderProgram *sp, PointCloud pc)
{
	this->shader = sp;
	this->pointcloud = pc;
}

void PointCloudScene::Start()
{
	shader->Start();
	shader->LoadModel(pointcloud);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	camera.position = vec3(0, 0, 2);
	camera.up = vec3(0, 1, 0);
	camera.lookAt = vec3(0, 0, 0);
	camera.viewSize = 5;
	camera.nearClip = 0.1f;
	camera.farClip = 20.0f;
	camera.projType = Camera::ORTHO;
	background.camera = &camera;
	background.Start();
	cont.camera = &camera;
	cont.moveSpeed = 2.0f;
	cont.Start();
}

void PointCloudScene::Update(float deltaTime)
{
	cont.Update(deltaTime);
}

void PointCloudScene::Render()
{
	glDisable(GL_DEPTH_TEST);
	background.Render();
	glEnable(GL_DEPTH_TEST);
	shader->Draw();
}

void PointCloudScene::End()
{
	shader->End();
}