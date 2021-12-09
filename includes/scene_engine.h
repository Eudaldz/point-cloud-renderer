#pragma once
#include "point_cloud.h"
#include "renderer.h"
#include "camera.h"
#include "view_controller.h"
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
	ViewController viewCont;
	
	
	ColorShade colorShade;
	RenderOptions renderOptions;
	PointSizeFunc pointSizeFunc;
	RenderOptions initialRenderOptions;
	
	Renderer* currentRender = nullptr;
	Renderer* pointR = nullptr;
	Renderer* splatFtbApproxR = nullptr;
	Renderer* splatFtbAccurateR = nullptr;
	Renderer* splatBtfApproxR = nullptr;
	Renderer* splatBtfAccurateR = nullptr;

	void cycleRenderMode();
	void cycleSplatMode();
	void cycleSortMode();
	void cyclePointSizeFunc();
	void cycleColorShade();
	void resetRenderOptions();

	void update();

public:
	SceneEngine(PointCloud& pointcloud, PointSizeFunc initialPSizeFunc, ColorShade initialColorShade, RenderOptions initialRenderMode);
	void Run();
};
