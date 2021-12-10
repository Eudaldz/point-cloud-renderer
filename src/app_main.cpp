#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <optional>
#include "renderer.h"
#include "point_cloud.h"
#include "file_reader.h"
#include "scene_engine.h"
#include "input.h"
#include "point_cloud_primitives.h"
#include "linalg.h"
#include "glm/glm.hpp"

using namespace std;

constexpr char* DEFAULT_POINTCLOUD = "cube-solid-opaque";
constexpr char* DEFAULT_RENDERER = "point";
constexpr char* DEFAULT_COLOR_SHADE = "white";
constexpr char* DEFAULT_PSIZE_FUNC = "local-average";

constexpr char* USAGE_STRING = "";

constexpr int DEFAULT_VIEWPORTX = 960;
constexpr int DEFAULT_VIEWPORTY = 720;

namespace {
	GLFWwindow* window = nullptr;
	SceneEngine* sceneEngine = nullptr;
}

constexpr float ASPECT_RATIO = 4.0f / 3.0f;

enum class ParamaterValue {
	RENDER, COLOR_SHADE, POINT_SIZE, 
	VIEWPORT_X, VIEWPORT_Y,
	NONE
};


int openWindow(int screen_width, int screen_height);
void closeWindow();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void run();

optional<RenderOptions> parseRenderString(string renderStr);
optional<ColorShade> parseColorString(string colorShadeStr);
optional<PointSizeFunc> parsePSizeFunc(string pSizeFuncStr);
optional<PointCloud*> openPointCloud(string pointCloudStr, bool file);

int main2(int argc, char* argv[]) {
	std::vector<vec3> p(3);
	p[0] = vec3(0, 0, 0);
	p[1] = vec3(1, 0, 0);
	p[2] = vec3(0, 0, 1);
	vec3 normal;
	float curvature;
	LINALG::SurfaceFeatures(p, normal, curvature);
	cout << normal.x << " " << normal.y << " " << normal.z;
	return 0;
}

int main(int argc, char* argv[]) {
	string pointCloudStr = DEFAULT_POINTCLOUD;
	string renderStr = DEFAULT_RENDERER;
	string colorShadeStr = DEFAULT_COLOR_SHADE;
	string psizeFuncStr = DEFAULT_PSIZE_FUNC;
	bool file = false;
	
	int viewportX = DEFAULT_VIEWPORTX;
	int viewportY = DEFAULT_VIEWPORTY;

	ParamaterValue paramValue = ParamaterValue::NONE;
	string prev_param;

	for (int i = 1; i < argc; i++) {
		string param = argv[i];
		if (param[0] == '-') {
			//PARAMETER KEY
			if (paramValue != ParamaterValue::NONE) {
				cout << "Expected value for paramater: " << prev_param << endl;
				return 0;
			}
			
			if (param == "-r" || param == "--render") {
				paramValue = ParamaterValue::RENDER;
			}
			else if (param == "-p" || param == "--point-size-f") {
				paramValue = ParamaterValue::POINT_SIZE;
			}
			else if (param == "-c" || param == "--color") {
				paramValue = ParamaterValue::COLOR_SHADE;
			}
			else if (param == "-v" || param == "--viewport") {
				paramValue = ParamaterValue::VIEWPORT_X;
			}
			else if (param == "-f" || param == "--file") {
				file = true;
			}
			
			else if (param == "-h" || param == "--help") {
				cout << USAGE_STRING << endl;
				return 0;
			}
			else {
				cout << "Urecongnized paramater: " << param << endl;
				return 0;
			}
			prev_param = param;
		}
		else {
			//PARAMETER VALUE
			switch (paramValue) {
			case ParamaterValue::RENDER:
				renderStr = param;
				break;
			case ParamaterValue::COLOR_SHADE:
				colorShadeStr = param;
				break;
			case ParamaterValue::POINT_SIZE:
				psizeFuncStr = param;
				break;
			case ParamaterValue::VIEWPORT_X:
				paramValue = ParamaterValue::VIEWPORT_Y;
				try {
					viewportX = std::stoi(param);
				}
				catch (std::invalid_argument e) {
					cout << "Paramater -v expects two integers" << endl;
					return 0;
				}
				break;
			case ParamaterValue::VIEWPORT_Y:
				try {
					viewportY = std::stoi(param);
				}
				catch (std::invalid_argument e) {
					cout << "Paramater -v expects two integers" << endl;
					return 0;
				}
				break;
			case ParamaterValue::NONE:
				pointCloudStr = param;
				break;
			}
		}
	}
	bool error = false;
	auto optRender = parseRenderString(renderStr);
	auto optColor = parseColorString(colorShadeStr);
	auto optPSize = parsePSizeFunc(psizeFuncStr);
	auto optPointCloud = openPointCloud(pointCloudStr, file);
	if (!optRender.has_value()) {
		cout << "Invalid render paramater!" << endl;
		error = true;
	}
	if (!optColor.has_value()) {
		cout << "Invalid color shading paramater!" << endl;
		error = true;
	}
	if (!optPSize.has_value()) {
		cout << "Invalid point size function paramater!" << endl;
		error = true;
	}
	if (!optPointCloud.has_value()) {
		error = true;
	}
	if (error)return 0;
	openWindow(viewportX, viewportY);
	Input::TrackWindow(window);
	PointCloud* pointCloud = optPointCloud.value();
	sceneEngine = new SceneEngine(*pointCloud, optPSize.value(), optColor.value(), optRender.value());
	
	run();
	
	delete sceneEngine;
	delete pointCloud;
	closeWindow();
	return 0;
}

void run()
{
	float deltaTime = 0;
	float lastFrame = 0;
	float currentFrame = 0;
	float fpsCount = 0;
	float sec = 0;
	Input::Poll();
	std::cout << "\nRUNNING...\n\n";
	while (!Input::ExitRequest()) {
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
		}
		else {
			fpsCount += 1;
		}
		sceneEngine->Draw(deltaTime);
		glfwSwapBuffers(window);
		Input::Poll();
	}
}

optional<RenderOptions> parseRenderString(string renderStr)
{
	if (renderStr == "point") {
		return RenderOptions(RenderMode::POINT, SplatMode::FTB, SortMode::APPROX);
	}
	else if (renderStr == "splat") {
		return RenderOptions(RenderMode::SPLAT, SplatMode::FTB, SortMode::APPROX);
	}
	else if (renderStr == "splat-ftb") {
		return RenderOptions(RenderMode::SPLAT, SplatMode::FTB, SortMode::APPROX);
	}
	else if (renderStr == "splat-btf") {
		return RenderOptions(RenderMode::SPLAT, SplatMode::BTF, SortMode::APPROX);
	}
	else if (renderStr == "splat-ftb-approx") {
		return RenderOptions(RenderMode::SPLAT, SplatMode::FTB, SortMode::APPROX);
	}
	else if (renderStr == "splat-ftb-accurate") {
		return RenderOptions(RenderMode::SPLAT, SplatMode::FTB, SortMode::ACCURATE);
	}
	else if (renderStr == "splat-btf-approx") {
		return RenderOptions(RenderMode::SPLAT, SplatMode::BTF, SortMode::APPROX);
	}
	else if (renderStr == "splat-btf-accurate") {
		return RenderOptions(RenderMode::SPLAT, SplatMode::BTF, SortMode::ACCURATE);
	}
	return nullopt;
}

optional<ColorShade> parseColorString(string colorShadeStr)
{
	if (colorShadeStr == "white") {
		return ColorShade(ColorMode::WHITE, ShadeMode::NO_SHADE);
	}
	else if (colorShadeStr == "white-shaded") {
		return ColorShade(ColorMode::WHITE, ShadeMode::SHADE);
	}
	else if (colorShadeStr == "color") {
		return ColorShade(ColorMode::COLOR, ShadeMode::NO_SHADE);
	}
	else if (colorShadeStr == "color-shaded") {
		return ColorShade(ColorMode::COLOR, ShadeMode::SHADE);
	}
	else if (colorShadeStr == "normal") {
		return ColorShade(ColorMode::NORMAL, ShadeMode::NO_SHADE);
	}
	else if (colorShadeStr == "normal-shaded") {
		return ColorShade(ColorMode::NORMAL, ShadeMode::SHADE);
	}
	else if (colorShadeStr == "curvature") {
		return ColorShade(ColorMode::CURVATURE, ShadeMode::NO_SHADE);
	}
	else if (colorShadeStr == "curvature-shaded") {
		return ColorShade(ColorMode::CURVATURE, ShadeMode::SHADE);
	}
	else if (colorShadeStr == "label") {
		return ColorShade(ColorMode::LABEL, ShadeMode::NO_SHADE);
	}
	else if (colorShadeStr == "label-shaded") {
		return ColorShade(ColorMode::LABEL, ShadeMode::SHADE);
	}
	else if (colorShadeStr == "pred") {
		return ColorShade(ColorMode::LABEL, ShadeMode::NO_SHADE);
	}
	else if (colorShadeStr == "pred-shaded") {
		return ColorShade(ColorMode::PRED, ShadeMode::SHADE);
	}
	return nullopt;
}

optional<PointSizeFunc> parsePSizeFunc(string pSizeFuncStr)
{
	if (pSizeFuncStr == "individual") {
		return PointSizeFunc::Individual;
	}
	else if (pSizeFuncStr == "local-average") {
		return PointSizeFunc::LocalAverage;
	}
	else if (pSizeFuncStr == "total-average") {
		return PointSizeFunc::TotalAverage;
	}
	return nullopt;
}

optional<PointCloud*> openPointCloud(string pointCloudStr, bool file)
{
	if (file) {
		vector<Point> points;
		Reader::Options options;
		if (Reader::ReadPointCloud(pointCloudStr, options, points) == -1) {
			cout << "Point Cloud file could not be read!" << endl;
			return nullopt;
		}
		return new PointCloud(points, true);
	}
	else {
		if (pointCloudStr == "slice") {
			return PCPrimitives::slice(32);
		}
		else if (pointCloudStr == "slice-transparent") {
			return PCPrimitives::slice_transparent(32);
		}
		else if (pointCloudStr == "slice-noisy") {
			return PCPrimitives::slice_noisy(32);
		}
		else if (pointCloudStr == "star") {
			return PCPrimitives::star(32);
		}
		else if (pointCloudStr == "star-noisy") {
			return PCPrimitives::star_noisy(32);
		}
		else if (pointCloudStr == "cube-surface") {
			return PCPrimitives::cube_surface(32);
		}
		else if (pointCloudStr == "sphere-surface") {
			return PCPrimitives::sphere_surface(32);
		}
		else if (pointCloudStr == "cube-solid-opaque") {
			return PCPrimitives::cube_solid_opaque(32);
		}
		else if (pointCloudStr == "sphere-solid-opaque") {
			return PCPrimitives::sphere_solid_opaque(32);
		}
		else if (pointCloudStr == "cube-solid-transparent") {
			return PCPrimitives::cube_solid_transparent(32);
		}
		else if (pointCloudStr == "sphere-solid-transparent") {
			return PCPrimitives::sphere_solid_transparent(32);
		}
		else {
			cout << "Invalid Point Cloud name!" << endl;
			return nullopt;
		}
	}
	return nullopt;
}

int openWindow(int screen_width, int screen_height)
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
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		closeWindow();
		return -1;
	}
	glViewport(0, 0, screen_width, screen_height);
	return 0;
}

void closeWindow()
{
	glfwTerminate();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	//TODO
	//glViewport(0, 0, (int)(height * ASPECT_RATIO), height);
}