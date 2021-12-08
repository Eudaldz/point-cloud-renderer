#pragma once
#include <glm/glm.hpp>

struct Point
{
	glm::vec3 position;
	glm::vec4 color;
	glm::vec3 normal;
	float curvature = 0;
	float pointSize = 0;
	uint32_t label = 0;
	uint32_t part = 0;

	Point() : position(0, 0, 0), color(0, 0, 0, 1), normal(0, 0, 0), pointSize(0), label(0), part(0)
	{}

	Point(glm::vec3 pos) : position(pos), color(0, 0, 0, 1), normal(0, 0, 0), pointSize(0), label(0), part(0)
	{}

	Point(glm::vec3 pos, glm::vec4 c) : position(pos), color(c), normal(0, 0, 0), pointSize(0), label(0), part(0)
	{}

	Point(glm::vec3 pos, glm::vec4 c, glm::vec3 n) : position(pos), color(c), normal(n), pointSize(0), label(0), part(0)
	{}
};