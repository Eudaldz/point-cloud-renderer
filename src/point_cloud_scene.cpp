#include "point_cloud_scene.h"
#include <iostream>

PointCloudScene::PointCloudScene(ShaderProgram *sp, PointCloud pc)
{
	this->shader = sp;
	this->pointcloud = pc;
	this->pointSize = 0;
}

void PointCloudScene::normalizePointCloud()
{
	vec3 center = pointcloud.center;
	float scale = 4.0f / pointcloud.radius;
	for (size_t i = 0; i < pointcloud.vn; i++) {
		vec3 p = pointcloud.points[i].position;
		pointcloud.points[i].position = (p - center) * scale;
	}
	pointcloud.estimatedPointSize *= scale;
	std::cout << "estimatedPointSize: " << this->pointcloud.estimatedPointSize << std::endl;
}

void PointCloudScene::Start()
{
	normalizePointCloud();
	shader->Start();
	shader->LoadModel(pointcloud);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	camera.position = vec3(0, 0, 5);
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
	background.Update(deltaTime);
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	pointSize = pointcloud.estimatedPointSize / camera.viewSize * (float)viewport[3];
}

void PointCloudScene::Render()
{
	glDisable(GL_DEPTH_TEST);
	background.Render();
	glEnable(GL_DEPTH_TEST);
	mat4 view = camera.GetViewMatrix();
	mat4 projection = camera.GetProjMatrix();
	mat4 model = mat4(1.0f);
	shader->SetTransforms(model, view, projection);
	shader->SetPointSize(pointSize);
	shader->Draw();
}

void PointCloudScene::End()
{
	shader->End();
}