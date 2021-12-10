#pragma once
#include "point_cloud.h"
#include "renderer.h"
#include "camera.h"
#include "view_controller.h"
#include "shaders/point_shader.h"
#include <glm/glm.hpp>

enum class RenderMode {
	POINT, SPLAT
};

enum class SplatMode {
	FTB, BTF
};

enum class SortMode {
	ACCURATE, APPROX 
};

struct RenderOptions {
	RenderMode renderMode;
	SplatMode splatMode;
	SortMode sortMode;
	RenderOptions(RenderMode render, SplatMode splat, SortMode sort) :renderMode(render), splatMode(splat), sortMode(sort) {};
};

class SceneEngine {
private:
	PointCloud& pointcloud;
	
	Camera camera;
	glm::mat4 modelT;
	float pSizeT;
	float pSizeMult = 1.0f;
	ViewController viewCont;
	
	
	ColorShade colorShade;
	RenderOptions renderOptions;
	PointSizeFunc pointSizeFunc;
	RenderOptions initialRenderOptions;
	
	Renderer* currentRender = nullptr;
	PointShader* pointRender = nullptr;
	//Renderer* splatFtbRender = nullptr;
	//Renderer* splatBtfRender = nullptr;

	void cycleRenderMode();
	void cycleSplatMode();
	void cycleSortMode();
	void cyclePointSizeFunc();
	void cycleShadeMode();
	void incrementColorMode();
	void decrementColorMode();
	void resetRenderOptions();
	void setRender();

	void update();


public:
	SceneEngine(PointCloud& pointcloud, PointSizeFunc initialPSizeFunc, ColorShade initialColorShade, RenderOptions initialRenderMode);
	void Draw(float deltaTime);
};
