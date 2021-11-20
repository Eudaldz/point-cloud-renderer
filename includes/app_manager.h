#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <point_cloud.h>
#include "shader_program.h"
#include "scene.h"

enum class Render { Default };

class AppManager
{
private:
	GLFWwindow* window;
	Scene* scene;
	
	
	int openWindow();
	void closeWindow();

	void run();
	int openPointCloud(const char* id, PointCloud*& outPointCloud);
	int parsePointSize(const char* str, PointSize& outPointSize);
	int parseShader(const char* str, ShaderName& outShaderName);

public:
	AppManager();
	int OpenPointcloud(const char* id, const char* render, const char* size);
	int OpenPointcloudDual(const char* id_1, const char* id_2, const char* render);
	int OpenDemo(const char* id);
	int RunTest(const char* id);

	int screen_width = 960;
	int screen_height = 720;
	float aspectRatio = 4.0f/3.0f;
};