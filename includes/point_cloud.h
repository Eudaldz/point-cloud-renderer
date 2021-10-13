#pragma once
#include <glm/glm.hpp>
#include "point.h"
#include "kdtree.h"

enum class PointSize {
	NearestMax, NearestAverage, NearestAdaptative, NearestAverageAdaptative
};

class PointCloud
{
public:
	Point* points = nullptr;
	size_t vn = 0;
	glm::vec3 center = glm::vec3(0, 0, 0);
	float radius = 0;
	KdTree tree;
	float averagePointSize;
	PointCloud(Point* points, size_t vn);
	void SetPointSize(PointSize ps);

private:
	void createTree();
	void calculateBounds();
	void calculatePointSize(PointSize ps);

	void setNearestMax();
	void setNearestAverage();
	void setNearestAdaptative();
	void setNearestAverageAdaptative();
};

namespace PCReader
{
	PointCloud* parseString(const char* str);
	PointCloud* parseFile(const char* fn);
}

namespace PCPrimitives
{
	constexpr size_t MAX_SAMPLE_RES = 1024;
	typedef glm::vec4 color_map(glm::vec3 coord);
	PointCloud* sample_star(size_t sampleRes);
	PointCloud* sample_star_noisy(size_t sampleRes);
	PointCloud* sample_slice(size_t sampleRes);
	PointCloud* sample_slice_noisy(size_t sampleRes);
	PointCloud* sample_cube_opaque(size_t sampleRes);
	PointCloud* sample_cube_opaque_noisy(size_t sampleRes);
	PointCloud* sample_cube_transparent(size_t sampleRes);
	PointCloud* sample_cube_transparent_noisy(size_t sampleRes);
}
