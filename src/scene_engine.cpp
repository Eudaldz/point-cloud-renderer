#include "scene_engine.h"
#include "point_cloud.h"
#include "input.h"
#include <iostream>

using namespace glm;

SceneEngine::SceneEngine(PointCloud& pointcloud, PointSizeFunc initialPSizeFunc,ColorShade initialColorShade, RenderOptions initialRenderOptions) 
	:pointcloud(pointcloud), pointSizeFunc(initialPSizeFunc), colorShade(initialColorShade), renderOptions(initialRenderOptions),
	viewCont(camera, modelT, pSizeMult), initialRenderOptions(initialRenderOptions)
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
	this->pointcloud.SetPointSize(pointSizeFunc);
	pointRender = new PointShader(this->pointcloud, camera, modelT, pSizeT, colorShade);
	setRender();
}

void SceneEngine::Draw(float deltaTime) 
{
	
	viewCont.Update(deltaTime);
	update();
	currentRender->Draw();
}

void SceneEngine::update() 
{
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	constexpr float u = 1.41421356237f;
	pSizeT = pSizeMult / camera.viewSize * (float)(viewport[3]);

	if (Input::GetButtonDown(Input::KEY_TAB)) {
		//CHANGE COLOR SHADE
		cycleShadeMode();
	}
	if (Input::GetButtonDown(Input::KEY_ARROW_UP)) {
		incrementColorMode();
	}
	if (Input::GetButtonDown(Input::KEY_ARROW_DOWN)) {
		decrementColorMode();
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
	setRender();
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
	setRender();
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
	setRender();
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
	pointcloud.SetPointSize(pointSizeFunc);
	currentRender->ReloadPointCloud();
}

void SceneEngine::cycleShadeMode()
{
	if (colorShade.shade == ShadeMode::NO_SHADE) {
		colorShade.shade = ShadeMode::SHADE;
	}
	else {
		colorShade.shade = ShadeMode::NO_SHADE;
	}
	currentRender->ChangeColorShade(colorShade);
}

void SceneEngine::incrementColorMode()
{
	switch (colorShade.color) {
	case ColorMode::WHITE:
		colorShade.color = ColorMode::COLOR;
		break;
	case ColorMode::COLOR:
		colorShade.color = ColorMode::LABEL;
		break;
	case ColorMode::LABEL:
		colorShade.color = ColorMode::PRED;
		break;
	case ColorMode::PRED:
		colorShade.color = ColorMode::NORMAL;
		break;
	case ColorMode::NORMAL:
		colorShade.color = ColorMode::CURVATURE;
		break;
	case ColorMode::CURVATURE:
		colorShade.color = ColorMode::WHITE;
		break;
	}
	currentRender->ChangeColorShade(colorShade);
}

void SceneEngine::decrementColorMode()
{
	switch (colorShade.color) {
	case ColorMode::WHITE:
		colorShade.color = ColorMode::CURVATURE;
		break;
	case ColorMode::COLOR:
		colorShade.color = ColorMode::WHITE;
		break;
	case ColorMode::LABEL:
		colorShade.color = ColorMode::COLOR;
		break;
	case ColorMode::PRED:
		colorShade.color = ColorMode::LABEL;
		break;
	case ColorMode::NORMAL:
		colorShade.color = ColorMode::PRED;
		break;
	case ColorMode::CURVATURE:
		colorShade.color = ColorMode::NORMAL;
		break;
	}
	currentRender->ChangeColorShade(colorShade);
}

void SceneEngine::resetRenderOptions()
{
	renderOptions = initialRenderOptions;
	setRender();
}

void SceneEngine::setRender()
{
	if (renderOptions.renderMode == RenderMode::POINT) {
		currentRender = pointRender;
		currentRender->Begin();
	}
	else{
		if (renderOptions.splatMode == SplatMode::FTB) {
			//currentRender = splatFtbRender;
			if (renderOptions.sortMode == SortMode::APPROX) {

			}
			else {

			}
		}
		else {
			//currentRender = splatBtfRender;
			if (renderOptions.sortMode == SortMode::APPROX) {

			}
			else {

			}
		}
	}
}
