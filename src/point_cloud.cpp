#include "point_cloud.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace glm;

PointCloud::PointCloud(Point* points, uint32_t vn)
{
	this->points = points;
	this->vn = vn;
	calculateBounds();
	createTree();
	calculateAveragePointDist();
}

void PointCloud::createTree()
{
	tree.model = points;
	tree.size = vn;
	tree.Construct();
}

void PointCloud::calculateBounds()
{
	vec3 c = vec3(0, 0, 0);
	for (uint32_t i = 0; i < vn; i++) {
		c += points[i].position;
	}
	center = c / (float)vn;

	float r = 0;
	for (uint32_t i = 0; i < vn; i++) {
		float tr = glm::distance(center, points[i].position);
		if (r < tr) r = tr;
	}
	radius = r;
}

void PointCloud::calculateAveragePointDist() 
{
	float sum = 0;
	uint32_t pi;
	for (uint32_t i = 0; i < vn; i++) {
		tree.NearestSearch(i, pi);
		sum += glm::distance(points[pi].position, points[i].position);
	}
	averagePointDist = sum / vn;
}

void PointCloud::SetPointSize(PointSize ps)
{
	switch (ps) {
	case PointSize::NearestMax:
		setNearestMax();
		return;
	case PointSize::NearestAverage:
		setNearestAverage();
		return;
	case PointSize::NearestAdaptative:
		setNearestAdaptative();
		return;
	case PointSize::KNearestAdaptative:
		setKNearestAdaptative();
		return;
	}
}



void PointCloud::setNearestMax() 
{
	float max = 0;
	float near;
	uint32_t pi;
	for (uint32_t i = 0; i < vn; i++) {
		tree.NearestSearch(i, pi);
		near = glm::distance(points[pi].position, points[i].position);
		if (max < near)max = near;
	}
	for (uint32_t i = 0; i < vn; i++) {
		points[i].pointSize = max;
	}
}

void PointCloud::setNearestAverage() 
{
	for (size_t i = 0; i < vn; i++) {
		points[i].pointSize = averagePointDist;
	}
}

void PointCloud::setNearestAdaptative() 
{
	uint32_t pi;
	for (size_t i = 0; i < vn; i++) {
		tree.NearestSearch(i, pi);
		points[i].pointSize = glm::distance(points[pi].position, points[i].position);
	}
}

void PointCloud::setKNearestAdaptative()
{
	/*vector<Point> v;
	float space = averagePointSize * 2;
	float max = 0;
	for (size_t i = 0; i < vn; i++) {
		tree.NearestKSearch(i, 4, v);
		for (int j = 0; j < 4; j++) {
			float d = glm::distance(v[j].position, points[i].position);
			if (d < space && max < d)max = d;
		}
		points[i].pointSize = max;
		v.clear();
	}*/
}

