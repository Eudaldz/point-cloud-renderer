#include "scene_engine.h"
#include "point_cloud.h"
#include "input.h"
#include <iostream>

using namespace glm;

SceneEngine::SceneEngine(PointCloud& pointcloud, PointSizeFunc initialPSizeFunc,ColorShade initialColorShade, RenderOptions initialRenderOptions) 
	:pointcloud(pointcloud), pointSizeFunc(pointSizeFunc), colorShade(initialColorShade), renderOptions(initialRenderOptions), 
	viewCont(camera, modelT, pSizeT), initialRenderOptions(initialRenderOptions)
{
	camera.position = vec3(0, 0, 4);
	camera.lookAt = vec3(0, 0, 0);
	camera.up = vec3(0, 1, 0);
	camera.viewSize = 5;
	camera.nearClip = 0.1f;
	camera.farClip = 20.0f;
	modelT = mat4(1.0f);
	pSizeT = 1.0;
	viewCont.SetCurrentAsDefault();
	pointcloud.SetPointSize(pointSizeFunc);
}

void SceneEngine::Run() 
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
		viewCont.Update(deltaTime);
		update();
		currentRender->Draw();
	}
}

void SceneEngine::update() 
{
	if (Input::GetButtonDown(Input::KEY_TAB)) {
		//CHANGE COLOR SHADE
		cycleColorShade();
	}
	if (Input::GetButtonDown(Input::KEY_Z)) {
		//CHANGE PSIZE FUNC
		cyclePointSizeFunc();
	}
	if (Input::GetButtonDown(Input::KEY_X)) {
		//CHANGE RENDER MODE
		cycleRenderMode();
	}
	if (Input::GetButtonDown(Input::KEY_1)) {
		//CHANGE SPLAT MODE
		cycleSplatMode();
	}
	if (Input::GetButtonDown(Input::KEY_2)) {
		//CHANGE SORT MODE
		cycleSortMode();
	}
	if (Input::GetButtonDown(Input::KEY_Y)) {
		//RESET RENDER
		resetRenderOptions();
	}
}

void SceneEngine::cycleRenderMode() 
{
	switch (renderOptions.renderMode) {
	case RenderMode::POINT:
		renderOptions.renderMode = RenderMode::SPLAT;
		break;
	case RenderMode::SPLAT:
		renderOptions.renderMode = RenderMode::POINT;
		break;
	}
}

void SceneEngine::cycleSplatMode()
{
	switch (renderOptions.splatMode) {
	case SplatMode::FTB:
		renderOptions.splatMode = SplatMode::BTF;
		break;
	case SplatMode::BTF:
		renderOptions.splatMode = SplatMode::FTB;
		break;
	}
}

void SceneEngine::cycleSortMode()
{
	switch (renderOptions.sortMode) {
	case SortMode::ACCURATE:
		renderOptions.sortMode = SortMode::APPROX;
		break;
	case SortMode::APPROX:
		renderOptions.sortMode = SortMode::ACCURATE;
		break;
	}
}

void SceneEngine::cyclePointSizeFunc() 
{
	switch (pointSizeFunc) {
	case PointSizeFunc::Individual:
		pointSizeFunc = PointSizeFunc::LocalAverage;
		break;
	case PointSizeFunc::LocalAverage:
		pointSizeFunc = PointSizeFunc::TotalAverage;
		break;
	case PointSizeFunc::TotalAverage:
		pointSizeFunc = PointSizeFunc::Individual;
		break;
	}
}

void SceneEngine::cycleColorShade()
{
	switch (colorShade) {
	case ColorShade::WHITE:
		colorShade = ColorShade::WHITE_SHADED;
		break;
	case ColorShade::WHITE_SHADED:
		colorShade = ColorShade::COLOR;
		break;
	case ColorShade::COLOR:
		colorShade = ColorShade::COLOR_SHADED;
		break;
	case ColorShade::COLOR_SHADED:
		colorShade = ColorShade::NORMAL;
		break;
	case ColorShade::NORMAL:
		colorShade = ColorShade::CURVATURE;
		break;
	case ColorShade::CURVATURE:
		colorShade = ColorShade::WHITE;
		break;
	}
}

void SceneEngine::resetRenderOptions()
{
	renderOptions = initialRenderOptions;
}
