#pragma once
#include "kdtree.h"
#include "tests/test.h"
#include "point_cloud.h"
#include <glm/glm.hpp>
#include <vector>

class KdTreeTest : public Test
{
private:
	KdTree tree;
	Point* model;
	uint32_t size;
	void generateModel();
	void testNN();
	void testAR();
	uint32_t baseNN(uint32_t i);
	void baseAxisRange(glm::vec3 axis, float minP, float maxP, std::vector<uint32_t> &buff);
public:
	KdTreeTest();
	void Run();
};