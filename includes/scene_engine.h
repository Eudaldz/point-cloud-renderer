#pragma once
#include "point_cloud.h"
#include "renderer.h"

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
	RenderMode render;
	SplatMode splat;
	SortMode sort;
	RenderOptions(RenderMode render, SplatMode splat, SortMode sort) :render(render), splat(splat), sort(sort) {};
};

class SceneEngine {
private:
	PointCloud& pointcloud;
public:
	SceneEngine(PointCloud& pointcloud);
	SetInitialState(ColorShade color, RenderOptions render);
	Run();
};
