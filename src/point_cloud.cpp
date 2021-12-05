#include "point_cloud.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <valarray>

using namespace std;
using namespace glm;


PointCloud::PointCloud(std::vector<Point>& pts)
{
	points.clear();
	points = std::move(pts);
	if (points.size() > MAX_POINTS)points.resize(MAX_POINTS);
	filter();
	standardize();
	calculateNeighbourSizes();
}

void PointCloud::filter()
{
	KdTree kt;
	kt.SetData(points.data(), points.size());
	valarray<float> va(points.size());
	for (uint32_t i = 0; i < points.size(); i++) {
		va[i] = kt.NearestDist(i);
	}
	float n = points.size();
	float sum = 0;
	float sqSum = 0;
	if (n == 1)++n; //AVOID 0 DIVISION IN STANDARD DEVIATION
	for (uint32_t i = 0; i < points.size(); i++) {
		float x = va[i];
		sum += x;
		sqSum += x * x;
	}
	
	float mean = sum / n;
	float stdDev = sqrt((sqSum - n * mean * mean) / (n - 1));
	uint32_t fEnd = points.size();
	for (uint32_t i = 0; i < fEnd;) {
		float zscore = abs((va[i] - mean) / stdDev);
		if (zscore >= 3) {
			--fEnd;
			Point t1 = points[fEnd];
			points[fEnd] = points[i];
			points[i] = t1;
		}
		else {
			++i;
		}
	}
	if(fEnd < points.size())points.resize(fEnd);
}

void PointCloud::standardize()
{
	vec3 center(0, 0, 0);
	for (uint32_t i = 0; i < points.size(); i++) {
		center += points[i].position;
	}
	center /= points.size();
	float radius = 0;
	for (uint32_t i = 0; i < points.size(); i++) {
		points[i].position -= center;
		float d = glm::length(points[i].position);
		if (radius < d)radius = d;
	}
	for (uint32_t i = 0; i < points.size(); i++) {
		points[i].position /= radius;
	}
}

void PointCloud::calculateNeighbourSizes()
{

}

/*
PointCloud::PointCloud(Point* points, uint32_t vn)
{
	this->points = points;
	this->vn = vn;
	calculateBounds();
	createTree();
	createAxialProjections();
	calculateAveragePointDist();
}

void PointCloud::createAxialProjections()
{
	axialProjections.model = points;
	axialProjections.size = vn;
	axialProjections.ConstructAxes();
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
	vector<Point> v;
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
	}
}*/

