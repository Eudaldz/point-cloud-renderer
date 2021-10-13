#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <point_cloud.h>

#include "scene.h"

enum class Render { Default };

class AppManager
{
private:
	GLFWwindow* window;
	Scene* scene;
	float deltaTime;
	float lastFrame;
	float fpsCount;
	float sec;
	
	int openWindow();
	void closeWindow();

	void run();
	PointCloud* openPointCloud(const char* id);
	PointSize parsePointSize(const char* str);

public:
	AppManager();
	int OpenPointcloud(const char* id, const char* render, const char* size);
	int OpenPointcloudDual(const char* id_1, const char* id_2, const char* render);
	int OpenDemo(const char* id);
	int RunTest(const char* id);

	int screen_width = 800;
	int screen_height = 600;
	float aspectRatio = 4.0f/3.0f;
};