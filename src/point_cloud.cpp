#include "point_cloud.h"
#include "point.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <valarray>
#include <list>
#include "linalg.h"

using namespace std;
using namespace glm;


PointCloud::PointCloud(std::vector<Point>& pts, bool recalculateNormals)
{
	points = std::move(pts);
	if (points.size() > MAX_POINTS)points.resize(MAX_POINTS);
	kdtree.SetData(points.data(), (uint32_t)points.size());
	
	removeEquals();
	filter();
	
	initializeElements();
	standardize();
	calculateNeighbourSizes();
	if (recalculateNormals)calculateNormals();
}

void PointCloud::removeEquals()
{
	//REMOVE EQUAL POINTS
	vector<bool> remove(points.size(), false);
	vector<uint32_t> zeros;
	for (uint32_t i = 0; i < points.size(); ++i) {
		if (!remove[i]) {
			kdtree.NearestRSearch(i, numeric_limits<float>().epsilon(), zeros);
			for (auto it = zeros.begin(); it != zeros.end(); ++it) {
				remove[*it] = true;
			}
		}
	}
	uint32_t tEnd = (uint32_t)points.size();
	for (uint32_t i = 0; i < tEnd;) {
		if (remove[i]) {
			--tEnd;
			std::swap(points[i], points[tEnd]);
			std::swap(remove[i], remove[tEnd]);
		}
		else {
			++i;
		}
	}
	points.resize(tEnd);
	kdtree.SetData(points.data(), (uint32_t)points.size());
}

void PointCloud::removeFar()
{
	//TODO
	vector<float> va(points.size());
	for (uint32_t i = 0; i < points.size(); i++) {
		va[i] = kdtree.NearestDist(i);
	}
}

void PointCloud::filter()
{
	//REMOVE FAR OUTLIERS

	
	/*
	if (points.size() < 2)return;
	KdTree kt;
	kt.SetData(points.data(), (uint32_t)points.size());
	vector<float> va(points.size());
	float epsilonCount = 0;
	for (uint32_t i = 0; i < points.size(); i++) {
		va[i] = kt.NearestDist(i);
		uint32_t j;
		kt.NearestSearch(i, j);
		if (va[i] <= std::numeric_limits<float>().epsilon()) {
			epsilonCount++;
			Point p1 = points[i];
			Point p2 = points[j];
			cout << "*";
		}
	}
	
	std::sort(va.begin(), va.end());

	float rangeMin = va.front();
	float rangeMax = va.back() + 1.0f/50.0f * (va.back() - va.front());
	uint32_t prevIndex = 0;
	cout <<"Distance distribution: "<<points.size() << endl;
	cout << "MIN: " << va.front() << endl <<"MAX: " << va.back()<< endl << endl;
	cout << "ZERO COUNT: " << epsilonCount << endl;
	for (uint32_t i = 0; i < 50; i++) {
		float fromRange = rangeMin + i * (rangeMax - rangeMin) / 50.0f;
		float toRange = rangeMin + (i+1) * (rangeMax - rangeMin) / 50.0f;
		uint32_t count = 0;
		while (prevIndex < points.size() && va[prevIndex] < toRange) {
			++count;
			++prevIndex;
		}
		cout << "[ " << fromRange << " - " << toRange << " ] --> " << count << endl;
	}
	

	/*float n = (float)points.size();
	float sum = 0;
	float sqSum = 0;
	
	for (uint32_t i = 0; i < points.size(); i++) {
		float x = va[i];
		sum += x;
		sqSum += x * x;
	}
	
	float mean = sum / n;
	float stdDev = sqrt((sqSum - n * mean * mean) / (n - 1));
	cout << "AVERAGE MIN DIST: " << mean << endl << "STANDARD DEVIATION: " << stdDev << endl;
	cout << "SMALLES MIN DIST: " << minimum << endl;
	uint32_t fEnd = (uint32_t)points.size();
	for (uint32_t i = 0; i < fEnd;) {
		float sZScore = (va[i] - mean) / stdDev;
		float zscore = abs(sZScore);
		if (zscore >= 3) {
			--fEnd;
			Point t1 = points[fEnd];
			float f1 = va[fEnd];
			points[fEnd] = points[i];
			points[i] = t1;
			va[fEnd] = va[i];
			va[i] = f1;
			if(sZScore < 0)cout << "SMALL OUTLIER DIST: " << va[i] << endl;
			if(sZScore > 0)cout << "BIG OUTLIER DIST: " << va[i] << endl;
			++i;
		}
		else {
			++i;
		}
	}
	if(fEnd < points.size())points.resize(fEnd);*/
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
	slices.push_back(PointCloudSlice(&elements[0], (uint32_t)points.size()));

}

void PointCloud::calculateNeighbourSizes()
{
	neighbourSizes.resize(points.size());
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
		//neighbourSizes[i] = glm::max(mean, minDist);
		neighbourSizes[i] = mean;
		individualSizes[i] = minDist;
		averageIndividualSize += minDist;
	}
	averageIndividualSize /= points.size();
	cout << "AVERAGE NEIGHBOUR MIN DIST: " << averageIndividualSize << endl;
}

void PointCloud::calculateNormals()
{
	vector<uint32_t> nears;
	vector<vec3> set;
	float averageK = 0.0f;
	for (uint32_t i = 0; i < points.size(); i++) {
		kdtree.NearestRSearch(i, neighbourSizes[i] * NEIGHBOUR_MULTIPLIER, nears);
		if (nears.size() < 3) {
			kdtree.NearestKSearch(i, 3, nears);
		}
		averageK += nears.size();
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
	averageK /= points.size();
	cout << "AVERAGE K NEARS: " << averageK << endl;
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

const vector<Point>& PointCloud::Points()
{
	return points;
}

const vector<uint32_t>& PointCloud::Elements()
{
	return elements;
}

const vector<PointCloudSlice>& PointCloud::Slices()
{
	return slices;
}

float PointCloud::AverageDist()
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
	uint32_t to = (uint32_t)points.size();
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

