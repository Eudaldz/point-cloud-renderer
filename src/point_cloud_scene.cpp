#include "point_cloud_scene.h"
#include "point_cloud.h"
#include <iostream>

PointCloudScene::PointCloudScene(ShaderProgram *sp, PointCloud* pc)
{
	this->shader = sp;
	this->pointcloud = pc;
	this->pointSizeTransform = 0;
}

void PointCloudScene::normalizePointCloud()
{
	/*vec3 center = pointcloud->center;
	float scale = 4.0f / pointcloud->radius;
	for (size_t i = 0; i < pointcloud->vn; i++) {
		vec3 p = pointcloud->points[i].position;
		pointcloud->points[i].position = (p - center) * scale;
		pointcloud->points[i].pointSize *= scale;
	}
	pointcloud->averagePointSize *= scale;*/
}

void PointCloudScene::Start()
{
	normalizePointCloud();
	shader->Start();
	shader->LoadModel(pointcloud);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
	camera.position = vec3(0, 0, 4);
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
	constexpr float u = 1.41421356237f;
	pointSizeTransform = 1.0f / camera.viewSize * (float)viewport[3];
}

void PointCloudScene::Render()
{
	background.Render();
	mat4 view = camera.GetViewMatrix();
	mat4 projection = camera.GetProjMatrix();
	mat4 model = mat4(1.0f);
	mat4 world = camera.GetWorldMatrix();
	if (camera.projType == camera.ORTHO) {
		mat4 camDir = mat4(0.0f);
		vec4 d = vec4(camera.lookAt - camera.position, 1.0f);
		camDir[3] = d;
		shader->SetCameraDir(camDir);
	}
	else {
		mat4 camDir = mat4(1.0f);
		camDir[3] = vec4(-camera.position, 1.0f);
		shader->SetCameraDir(camDir);
	}
	shader->SetTransforms(model, view, projection);
	shader->SetPointSizeTransform(pointSizeTransform);
	shader->SetCameraWorldView(world);
	
	//vec3 viewDir = glm::normalize(camera.lookAt - camera.position);
	//std::cout <<"camera: " << viewDir.x << " " << viewDir.y << " " << viewDir.z << std::endl;
	shader->Draw();
}

void PointCloudScene::End()
{
	shader->End();
}