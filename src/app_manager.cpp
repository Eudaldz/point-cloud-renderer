#include "app_manager.h"
#include <iostream>
#include "scene.h"
#include "input.h"
#include "shader_program.h"
#include "demo_scenes/demo_collection.h"
#include "point_cloud.h"
#include <string>
#include "point_cloud_scene.h"
#include "point_cloud_primitives.h"
#include "tests/test.h"

namespace
{
	float _aspectRatio = 4.0f/3.0f;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
bool load_glad();

AppManager::AppManager() 
{
	window = NULL;
	scene = NULL;
}

int AppManager::OpenPointcloud(const char* id, const char* render, const char* size)
{
	ShaderProgram* sp;
	PointSize ps; 
	ShaderName sn;
	PointCloud* pc = nullptr;
	bool parseError = false;

	if (!parseShader(render, sn)) {
		std::cout << "Shader program not found" << std::endl;
		parseError = true;
	}

	if (!parsePointSize(size, ps)) {
		std::cout << "Point size method not found" << std::endl;
		parseError = true;
	}

	if (!openPointCloud(id, pc)) {
		std::cout << "Point cloud not found" << std::endl;
		parseError = true;
	}

	if (parseError) {
		if (pc != nullptr)delete pc;
		return -1;
	}

	sp = ShaderProgram::NewShader(sn);
	pc->SetPointSize(ps);
	scene = new PointCloudScene(sp, pc);
	run();
	delete scene;
	delete sp;
	delete pc;
	return 0;
}

int AppManager::OpenPointcloudDual(const char* id_1, const char* id_2, const char* render)
{
	std::cout << "<TODO: render dual point cloud>";
	return 0;
}

int AppManager::OpenDemo(const char* id)
{
	scene = DemoCollection::OpenDemo(id);
	if (scene == NULL) {
		std::cout << "Demo not found";
		return -1;
	}
	run();
	delete scene;
	return 0;
}

int AppManager::RunTest(const char* id) 
{
	Test* t = Test::NewTest(id);
	t->Run();
	return 0;
}

void AppManager::run() 
{
	if (openWindow() == -1) {
		return;
	}
	float deltaTime = 0;
	float lastFrame = 0;
	float currentFrame = 0;
	float fpsCount = 0;
	float sec = 0;
	Input& in = Input::GetInstance();
	in.TrackWindow(window);
	in.PollInput();
	std::cout << "\nRUNNING...\n\n";
	scene->Start();
	while (!in.ExitRequest()) {
		currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		sec += deltaTime;
		if (sec > 1.0f) {
			float extra = sec - 1.0f;
			float used = deltaTime - extra;
			fpsCount += used / deltaTime;
			std::cout << "\nFPS:: " << fpsCount << std::endl;
			fpsCount = extra / deltaTime;
			sec = extra;
		} else {
			fpsCount += 1;
		}

		scene->Update(deltaTime);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		scene->Render();

		glfwSwapBuffers(window);
		in.PollInput();
	}
	scene->End();
	in.UntrackWindow();
	closeWindow();
}


int AppManager::openWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	if (window != NULL) {
		std::cout << "Window already open" << std::endl;
		return -1;
	}
	window = glfwCreateWindow(screen_width, screen_height, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	_aspectRatio = aspectRatio; //WORKARROUND TO BAD GLFW OOP DESING
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (!load_glad()) {
		closeWindow();
		return -1;
	}
	glViewport(0, 0, screen_width, screen_height);
	return 0;
}

void AppManager::closeWindow()
{
	glfwTerminate();
	this->window = NULL;
}

int AppManager::openPointCloud(const char* id, PointCloud*& outPointCloud)
{
	if (strcmp(id, "sample_slice") == 0) {
		outPointCloud = PCPrimitives::slice(128);
		return 1;
	}
	else if (strcmp(id, "sample_slice_transparent") == 0) {
		outPointCloud = PCPrimitives::slice_transparent(128);
		return 1;
	}
	else if (strcmp(id, "sample_slice_noisy") == 0) {
		outPointCloud = PCPrimitives::slice_noisy(128);
		return 1;
	}
	else if (strcmp(id, "sample_star") == 0) {
		outPointCloud = PCPrimitives::star(128);
		return 1;
	}
	else if (strcmp(id, "sample_star_noisy") == 0) {
		outPointCloud = PCPrimitives::star_noisy(128);
		return 1;
	}
	else if (strcmp(id, "sample_cube_opaque") == 0) {
		outPointCloud = PCPrimitives::cube_opaque(32);
		return 1;
	}
	else if (strcmp(id, "sample_cube_transparent") == 0) {
		outPointCloud = PCPrimitives::cube_transparent(32);
		return 1;
	}
	outPointCloud = nullptr;
	return 0;
}

int AppManager::parseShader(const char* str, ShaderName& outShaderName)
{
	if (strcmp(str, "") == 0) {
		outShaderName = ShaderName::Point;
		return 1;
	} else if (strcmp(str, "point") == 0) {
		outShaderName = ShaderName::Point;
		return 1;
	}else if (strcmp(str, "splat") == 0) {
		outShaderName = ShaderName::Splat;
		return 1;
	}else if (strcmp(str, "layered_splat") == 0) {
		outShaderName = ShaderName::LayeredSplat;
		return 1;
	}
	return 0;
}

int AppManager::parsePointSize(const char* str, PointSize& outPointSize)
{
	if (strcmp(str, "") == 0) {
		outPointSize = PointSize::NearestAdaptative;
		return 1;
	}
	else if (strcmp(str, "nearest_max") == 0) {
		outPointSize = PointSize::NearestMax;
		return 1;
	}
	else if (strcmp(str, "nearest_average") == 0) {
		outPointSize = PointSize::NearestAverage;
		return 1;
	}
	else if (strcmp(str, "nearest_adaptative") == 0) {
		outPointSize = PointSize::NearestAdaptative;
		return 1;
	}
	else if (strcmp(str, "knearest_adaptative") == 0) {
		outPointSize = PointSize::KNearestAdaptative;
		return 1;
	}
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, (int)(height*_aspectRatio), height);
}

bool load_glad()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	return true;
}