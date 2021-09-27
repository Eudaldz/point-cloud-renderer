#pragma once
#include "kdtree.h"
#include "tests/test.h"
#include "point_cloud.h"

class KdTreeTest : public Test
{
private:
	KdTree tree;
	Point* model;
	size_t size;
	void generateModel();
	void testNN();
	size_t baseNN(size_t i);
public:
	KdTreeTest();
	void Run();
};