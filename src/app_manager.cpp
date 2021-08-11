#include "app_manager.h"
#include <iostream>
#include "scene.h"
#include "input.h"
#include "demo_scenes/demo_collection.h"

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
	deltaTime = 0;
	lastFrame = 0;
	fpsCount = 0;
	sec = 0;
}

int AppManager::OpenPointcloud(const char* id, Render render)
{
	std::cout << "<TODO: render point cloud>";
	return 0;
}

int AppManager::OpenPointcloudDual(const char* id_1, const char* id_2, Render render)
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

void AppManager::run() 
{
	if (openWindow() == -1) {
		return;
	}
	Input& in = Input::GetInstance();
	in.TrackWindow(window);
	in.PollInput();
	std::cout << "\nRUNNING...\n\n";
	scene->Start();
	while (!in.ExitRequest()) {
		float currentFrame = (float)glfwGetTime();
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