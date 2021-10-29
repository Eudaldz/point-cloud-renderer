#pragma once
#include <glm/glm.hpp>
#include "point.h"
#include "kdtree.h"

enum class PointSize {
	NearestMax, NearestAverage, NearestAdaptative, KNearestAdaptative  
};

class PointCloud
{
public:
	Point* points = nullptr;
	uint32_t vn = 0;
	glm::vec3 center = glm::vec3(0, 0, 0);
	float radius = 0;
	KdTree tree;
	float averagePointDist;
	PointCloud(Point* points, uint32_t vn);
	void SetPointSize(PointSize ps);

private:
	void createTree();
	void calculateBounds();
	void calculateAveragePointDist();

	void setNearestMax();
	void setNearestAverage();
	void setNearestAdaptative();
	void setKNearestAdaptative();
};
