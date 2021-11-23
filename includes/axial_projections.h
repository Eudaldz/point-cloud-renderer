#pragma once

#include "point.h"
#include <glm/glm.hpp>
#include <vector>

using namespace glm;

struct ProjectionSlice {
	uint32_t* buffer;
	uint32_t count;
};

struct AxSlices {
	const std::vector<ProjectionSlice> *slices;
	bool reversed;
};

class AxialProjections {
private:
	vec3 directions[13] = {
		vec3(1,0,0),
		vec3(0,1,0),
		vec3(0,0,1),
		
		vec3(1,1,0),
		vec3(1,0,1),
		vec3(0,1,1),
		vec3(1,-1,0),
		vec3(1,0,-1),
		vec3(0,1,-1),
		
		vec3(1,1,1),
		vec3(1,-1,-1),
		vec3(1,1,-1),
		vec3(1,-1,1)
	};

	uint32_t* sorted_axis[13];
	std::vector<ProjectionSlice> slices[13];

	struct CompareProjection {
		Point* model;
		vec3 dir;

		bool operator() (uint32_t i, uint32_t j) { 
			return glm::dot(model[i].position, dir) < glm::dot(model[j].position, dir);
		}
	}compareProjection;


public:
	Point* model = nullptr;
	uint32_t size = 0;
	
	AxialProjections();
	void ConstructAxes();
	void ConstructSlices(float sliceDepth);
	AxSlices GetSlices(vec3 axis);

};