#include "point_cloud.h"
#include "point.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <valarray>
#include "linalg.h"

using namespace std;
using namespace glm;


PointCloud::PointCloud(std::vector<Point>& pts, bool recalculateNormals)
{
	points.clear();
	points = std::move(pts);
	if (points.size() > MAX_POINTS)points.resize(MAX_POINTS);
	filter();
	initializeElements();
	standardize();
	kdtree.SetData(points.data(), points.size());
	calculateNeighbourSizes();
	if (recalculateNormals)calculateNormals();
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

void PointCloud::initializeElements() {
	elements.resize(points.size());
	for (uint32_t i = 0; i < points.size(); ++i) {
		elements[i] = i;
	}
	slices.push_back(PointCloudSlice(&elements[0], points.size()));

}

void PointCloud::calculateNeighbourSizes()
{
	neighbourSizes.clear();
	neighbourSizes.resize(points.size());
	individualSizes.clear();
	individualSizes.resize(points.size());
	averageIndividualSize = 0;
	vector<uint32_t> nears;
	nears.reserve(K);
	for (uint32_t i = 0; i < points.size(); i++) {
		kdtree.NearestKSearch(i, K, nears);
		float minDist = 0;
		if(nears.size() >= 1)minDist = glm::distance(points[i].position, points[nears[0]].position);
		float mean = minDist;
		for (uint32_t j = 0; j < nears.size(); j++) {
			mean += kdtree.NearestDist(nears[j]);
		}
		mean /= (nears.size() + 1);
		neighbourSizes[i] = mean;
		individualSizes[i] = minDist;
		averageIndividualSize += minDist;
	}
	averageIndividualSize /= points.size();
}

void PointCloud::calculateNormals()
{
	vector<uint32_t> nears;
	vector<vec3> set;
	for (uint32_t i = 0; i < points.size(); i++) {
		kdtree.NearestRSearch(i, neighbourSizes[i] * NEIGHBOUR_MULTIPLIER, nears);
		set.clear();
		set.push_back(points[i].position);
		for (uint32_t j = 0; j < nears.size(); j++) {
			set.push_back(points[nears[j]].position);
		}
		vec3 normal;
		float curvature;
		LINALG::SurfaceFeatures(set, normal, curvature);
		points[i].normal = normal;
		points[i].curvature = curvature;
	}
}

void PointCloud::SetPointSize(PointSizeFunc psf) 
{
	switch (psf) {
	case PointSizeFunc::Individual:
		for (uint32_t i = 0; i < points.size(); i++) {
			points[i].pointSize = individualSizes[i];
		}
		break;
	case PointSizeFunc::LocalAverage:
		for (uint32_t i = 0; i < points.size(); i++) {
			points[i].pointSize = neighbourSizes[i];
		}
		break;
	case PointSizeFunc::TotalAverage:
		for (uint32_t i = 0; i < points.size(); i++) {
			points[i].pointSize = averageIndividualSize;
		}
		break;
	}
}

inline const vector<Point>& PointCloud::Points()
{
	return points;
}

inline const vector<uint32_t>& PointCloud::Elements()
{
	return elements;
}

inline const vector<PointCloudSlice>& PointCloud::Slices()
{
	return slices;
}

inline float PointCloud::AverageDist()
{
	return averageIndividualSize;
}

void PointCloud::Sort(const glm::vec3& sortAxis, float slice_depth)
{
	std::sort(elements.begin(), elements.end(),
		[&sortAxis, this](uint32_t i, uint32_t j) {
			return glm::dot(points[i].position, sortAxis) < glm::dot(points[j].position, sortAxis);
		});
	float begin = glm::dot(points[elements.front()].position, sortAxis) + slice_depth/2.0f;
	float end = glm::dot(points[elements.back()].position, sortAxis) + slice_depth + slice_depth/2.0f;
	slices.clear();
	uint32_t from = 0;
	for (float s = begin; s < end; s += slice_depth) {
		uint32_t to = elementBinarySearch(s, sortAxis);
		slices.push_back(PointCloudSlice(&elements[from], to - from));
		from = to;
	}
}

void PointCloud::HalfSort(const glm::vec3& sortAxis, float slice_depth)
{
	//TODO: Implement
	Sort(sortAxis, slice_depth);
}

uint32_t PointCloud::elementBinarySearch(float s, const glm::vec3& sortAxis)
{
	uint32_t from = 0;
	uint32_t to = points.size();
	while (from < to) {
		uint32_t ind = (to - from) / 2;
		float v = glm::dot(points[elements[ind]].position, sortAxis);
		if (v < s) {
			from = ind;
		}
		else {
			to = ind;
		}
	}
	return from;
}

