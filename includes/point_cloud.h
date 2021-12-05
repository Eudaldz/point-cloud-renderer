#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <limits>
#include <valarray>
#include "point.h"
#include "kdtree.h"
#include "axial_projections.h"

enum class PointSizeFunc {
	NearestMax, NearestAverage, NearestAdaptative, KNearestAdaptative  
};

struct PointCloudSlice {
	Point* begin;
	Point* end;
	size_t size;
};

class PointCloud
{
private:
	static constexpr uint32_t MAX_POINTS = std::numeric_limits<uint32_t>::max();
	std::vector<Point> points;
	std::vector<float> neighbourSizes;
	float averageDist;

public:
	PointCloud(std::vector<Point>& points);
	//void SetPointSize(PointSizeFunc psf);
	//const Point* begin();
	//const Point* end();
	//size_t size();
	//float averageDist();

private:
	void filter();
	void standardize();
	void calculateNeighbourSizes();
};
