#pragma once
#include <glm/glm.hpp>

struct Point
{
	glm::vec3 position;
	glm::vec4 color;
	glm::vec3 normal;
	float pointSize;

	Point() : position(0,0,0), color(0,0,0,1), normal(0,0,0), pointSize(0)
	{}

	Point(glm::vec3 pos) : position(pos), color(0,0,0,1), normal(0,0,0), pointSize(0)
	{}

	Point(glm::vec3 pos, glm::vec4 c) : position(pos), color(c), normal(0,0,0), pointSize(0)
	{}

	Point(glm::vec3 pos, glm::vec4 c, glm::vec3 n) : position(pos), color(c), normal(n), pointSize(0)
	{}
};

struct PointCloud
{
	Point* points = nullptr;
	size_t vn = 0;
	float estimatedPointSize = 0;
	glm::vec3 center = glm::vec3(0,0,0);
	float radius = 0;
};

namespace PCReader
{
	PointCloud parseString(const char* str);
	PointCloud parseFile(const char* fn);
}

namespace PCPrimitives
{
	constexpr size_t MAX_SAMPLE_RES = 1024;
	typedef glm::vec4 color_map(glm::vec3 coord);
	PointCloud square(size_t sampleRes);
	PointCloud square(color_map *f, size_t sampleRes);
	PointCloud cube(size_t sampleRes);
	PointCloud cube(color_map* f, size_t sampleRes);
	PointCloud sphere(size_t sampleRes);
	PointCloud sphere(color_map* f, size_t sampleRes);
}
