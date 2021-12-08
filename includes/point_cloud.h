#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <limits>
#include <valarray>
#include <iterator>
#include "point.h"
#include "kdtree.h"

enum class PointSizeFunc {
	Individual, LocalAverage, TotalAverage
};

struct PointCloudSlice {
	uint32_t* begin;
	uint32_t count;
	PointCloudSlice(uint32_t* p, uint32_t count):begin(p), count(count){}
};

class PointCloud
{
private:
	static constexpr uint32_t K = 16;
	static constexpr float NEIGHBOUR_MULTIPLIER = 1.5f;
	static constexpr uint32_t MAX_POINTS = std::numeric_limits<uint32_t>::max();
	
	std::vector<Point> points;
	std::vector<uint32_t> elements;
	std::vector<PointCloudSlice> slices;
	KdTree kdtree;
	
	std::vector<float> neighbourSizes;
	std::vector<float> individualSizes;
	float averageIndividualSize;
	

public:
	PointCloud(std::vector<Point>& points, bool recalculateNormals = false);
	void SetPointSize(PointSizeFunc psf);
	const std::vector<Point>& Points();
	const std::vector<uint32_t>& Elements();
	const std::vector<PointCloudSlice>& Slices();

	float AverageDist();
	void Sort(const glm::vec3& sortAxis, float slice_depth);
	void HalfSort(const glm::vec3& sortAxis, float slice_depth);


private:
	void filter();
	void standardize();
	void calculateNeighbourSizes();
	void calculateNormals();
	void initializeElements();
	uint32_t elementBinarySearch(float s, const glm::vec3& sortAxis);
};
