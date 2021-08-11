#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

public:
	AppManager();
	int OpenPointcloud(const char* id, Render render = Render::Default);
	int OpenPointcloudDual(const char* id_1, const char* id_2, Render render = Render::Default);
	int OpenDemo(const char* id);

	int screen_width = 800;
	int screen_height = 600;
	float aspectRatio = 4.0f/3.0f;
};