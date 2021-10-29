#pragma once
#include "kdtree.h"
#include "tests/test.h"
#include "point_cloud.h"
#include <glm/glm.hpp>
#include <vector>

class KdTreeTest : public Test
{
private:
	static constexpr uint32_t TEST_COUNT = 1000;
	static constexpr uint32_t BENCHMARK_COUNT = 5000000;
	static constexpr uint32_t BENCHMARK_REPS = 100000;
	
	KdTree test_tree;
	KdTree benchmark_tree;
	
	Point* test_model;
	Point* benchmark_model;
	
	void generateTestModel();
	void generateBenchmarkModel();
	
	void testNN();
	void testKNN();
	void testMin();
	void testMax();
	void testAR();
	
	void baseNN(uint32_t i, uint32_t& result);
	void baseNN(glm::vec3 pos, uint32_t& result);
	void baseKNN(uint32_t i, unsigned int k, std::vector<uint32_t>& result);
	void baseKNN(glm::vec3 pos, unsigned int k, std::vector<uint32_t>& result);
	void baseMin(glm::vec3 axis, uint32_t& result, float& value);
	void baseMax(glm::vec3 axis, uint32_t& result, float& value);
	void baseAR(glm::vec3 axis, float from, float to, std::vector<uint32_t>& result);

	void benchmarkNN();
	void benchmarkAR();

public:
	KdTreeTest();
	void Run();
};