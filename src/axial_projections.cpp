#include "axial_projections.h"
#include <algorithm>

using namespace std;

AxialProjections::AxialProjections() 
{

}

void AxialProjections::ConstructAxes()
{
	if (size == 0)return;
	compareProjection.model = model;
	for (int i = 0; i < 13; i++) {
		vec3 dir = glm::normalize(directions[i]);
		directions[i] = dir;
		sorted_axis[i] = new uint32_t[size];
		for (uint32_t j = 0; j < size; j++) {
			sorted_axis[i][j] = j;
		}
		compareProjection.dir = dir;
		std::sort(sorted_axis[i], sorted_axis[i] + size, compareProjection);
	}
}

void AxialProjections::ConstructSlices(float sliceDepth)
{
	if (size == 0)return;
	for (int i = 0; i < 13; i++) {
		vec3 dir = directions[i];
		ProjectionSlice currentSlice;
		currentSlice.buffer = sorted_axis[i];
		currentSlice.count = 1;
		float sliceEnd = glm::dot(model[sorted_axis[i][0]].position, dir) + sliceDepth;
		for (uint32_t j = 1; j < size; j++) {
			float p = glm::dot(model[sorted_axis[i][j]].position, dir);
			if (p < sliceEnd) {
				currentSlice.count++;
			}
			else {
				slices[i].push_back(currentSlice);
				currentSlice.buffer = sorted_axis[i] + j;
				currentSlice.count = 1;
				sliceEnd = glm::dot(model[sorted_axis[i][j]].position, dir) + sliceDepth;
			}
		}
		slices[i].push_back(currentSlice);
	}
}

AxSlices AxialProjections::GetSlices(vec3 axis)
{
	axis = glm::normalize(axis);
	float bestProj = 0;
	int bestDir;
	for (int i = 0; i < 13; i++) {
		float d = abs(glm::dot(axis, directions[i]));
		if (d > bestProj) {
			bestProj = d;
			bestDir = i;
		}
	}
	bool reversed = glm::dot(axis, directions[bestDir]) < 0;
	AxSlices result;
	result.reversed = reversed;
	result.slices = &slices[bestDir];
	return result;
}