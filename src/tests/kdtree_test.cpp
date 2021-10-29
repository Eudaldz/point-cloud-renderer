#include "tests/kdtree_test.h"
#include <iostream>
#include <glm/glm.hpp>
#include <chrono>

using namespace std;
using namespace glm;
using namespace std::chrono;

KdTreeTest::KdTreeTest() 
{
	benchmark_model = nullptr;
	test_model = nullptr;
}

void KdTreeTest::Run() {
	std::srand((unsigned int)std::time(nullptr));
	generateTestModel();
	//testNN();
	//testKNN();
	//testMin();
	//testMax();
	//testAR();

	generateBenchmarkModel();
	benchmarkNN();
}

void KdTreeTest::generateTestModel() {
	cout << endl << endl << "...GENERATING TEST MODEL..." << endl << endl;
	test_model = new Point[TEST_COUNT];
	for (uint32_t i = 0; i < TEST_COUNT; i++) {
		float x = (float)rand() / (float)RAND_MAX;
		float y = (float)rand() / (float)RAND_MAX;
		float z = (float)rand() / (float)RAND_MAX;
		test_model[i].position = vec3(x, y, z);
	}
	cout << endl;
	test_tree.model = test_model;
	test_tree.size = TEST_COUNT;
	test_tree.Construct();
	cout << "DONE" << endl;
}

void KdTreeTest::generateBenchmarkModel() {
	cout << endl << endl << "...GENERATING BENCHMARK MODEL..." << endl << endl;
	benchmark_model = new Point[BENCHMARK_COUNT];
	for (uint32_t i = 0; i < BENCHMARK_COUNT; i++) {
		float x = (float)rand() / (float)RAND_MAX;
		float y = (float)rand() / (float)RAND_MAX;
		float z = (float)rand() / (float)RAND_MAX;
		benchmark_model[i].position = vec3(x, y, z);
	}
	cout << endl;
	benchmark_tree.model = benchmark_model;
	benchmark_tree.size = BENCHMARK_COUNT;
	benchmark_tree.Construct();
	cout << "DONE" << endl;
}

void KdTreeTest::testNN() 
{
	cout << endl << endl<<"::NEAREST NEIGHBOUR SEARCH TEST::" << endl << endl;
	bool pass = true;
	for (uint32_t i = 0; i < TEST_COUNT; i++) {
		float x = (float)rand() / (float)RAND_MAX;
		float y = (float)rand() / (float)RAND_MAX;
		float z = (float)rand() / (float)RAND_MAX;
		vec3 randomPos(x, y, z);
		uint32_t r1, r2, b1, b2;
		test_tree.NearestSearch(i, r1);
		test_tree.NearestSearch(randomPos, r2);
		baseNN(i, b1);
		baseNN(randomPos, b2);
		float d1 = glm::distance(test_model[i].position, test_model[r1].position);
		float d2 = glm::distance(test_model[i].position, test_model[b1].position);
		
		if (!(r1 == b1 && r2 == b2)) {
			pass = false;
			cout << "**ERROR**" << endl;
		}
	}
	if (pass) {
		cout << "All tests passed!" << endl;
	}
	else {
		cout << "Something is wrong!" << endl;
	}
	cout << "DONE" << endl;
}

void KdTreeTest::testKNN() 
{
	cout << endl << endl << "::K NEAREST NEIGHBOUR SEARCH TEST::" << endl << endl;
	bool pass = true;
	vector<uint32_t> r1, r2, b1, b2;
	for (uint32_t i = 0; i < TEST_COUNT; i++) {
		float x = (float)rand() / (float)RAND_MAX;
		float y = (float)rand() / (float)RAND_MAX;
		float z = (float)rand() / (float)RAND_MAX;
		vec3 randomPos(x, y, z);
		test_tree.NearestKSearch(i, 4, r1);
		test_tree.NearestKSearch(randomPos, 4, r2);
		baseKNN(i, 4, b1);
		baseKNN(randomPos, 4, b2);
		if (r1.size() == b1.size()) {
			for (int j = 0; j < r1.size(); j++) {
				if (test_model[r1[j]].position != test_model[b1[j]].position) {
					pass = false;
					cout << "**ERROR**" << endl;
					break;
				}
			}
		}
		else {
			cout << "**ERROR**" << endl;
			pass = false;
		}
		if (r2.size() == b2.size()) {
			for (int j = 0; j < r2.size(); j++) {
				if (test_model[r1[j]].position != test_model[b1[j]].position) {
					pass = false;
					cout << "**ERROR**" << endl;
					break;
				}
			}
		}
		else {
			cout << "**ERROR**" << endl;
			pass = false;
		}
	}
	if (pass) {
		cout << "All tests passed!" << endl;
	}
	else {
		cout << "Something is wrong!" << endl;
	}
	cout << "DONE" << endl;
}

void KdTreeTest::testMin() 
{
	cout << endl << endl << "::AXIS MINIMUM SEARCH TEST::" << endl << endl;
	bool pass = true;
	for (uint32_t i = 0; i < TEST_COUNT; i++) {
		float x = ((float)rand() / (float)RAND_MAX) * 2 - 1;
		float y = ((float)rand() / (float)RAND_MAX) * 2 - 1;
		float z = ((float)rand() / (float)RAND_MAX) * 2 - 1;
		vec3 randomAxis = glm::normalize(vec3(x, y, z));
		uint32_t r1, b1;
		float v1, bv1;
		test_tree.AxisMin(randomAxis, r1, v1);
		baseMin(randomAxis, b1, bv1);
		if (!(r1 == b1 && v1 == bv1)) {
			pass = false;
			cout << "**ERROR**" << endl;
		}
	}
	if (pass) {
		cout << "All tests passed!" << endl;
	}
	else {
		cout << "Something is wrong!" << endl;
	}
	cout << "DONE" << endl;
}

void KdTreeTest::testMax() 
{
	cout << endl << endl << "::AXIS MAXIMUM SEARCH TEST::" << endl << endl;
	bool pass = true;
	for (uint32_t i = 0; i < TEST_COUNT; i++) {
		float x = ((float)rand() / (float)RAND_MAX) * 2 - 1;
		float y = ((float)rand() / (float)RAND_MAX) * 2 - 1;
		float z = ((float)rand() / (float)RAND_MAX) * 2 - 1;
		vec3 randomAxis = glm::normalize(vec3(x, y, z));
		uint32_t r1, b1;
		float v1, bv1;
		test_tree.AxisMax(randomAxis, r1, v1);
		baseMax(randomAxis, b1, bv1);
		if (!(r1 == b1 && v1 == bv1)) {
			pass = false;
			cout << "**ERROR**" << endl;
		}
	}
	if (pass) {
		cout << "All tests passed!" << endl;
	}
	else {
		cout << "Something is wrong!" << endl;
	}
	cout << "DONE" << endl;
}

void KdTreeTest::testAR() 
{
	cout << endl << endl << "::AXIS RANGE SEARCH TEST::" << endl << endl;
	bool pass = true;
	vector<uint32_t> r1, b1;
	for (uint32_t i = 0; i < TEST_COUNT; i++) {
		float x = ((float)rand() / (float)RAND_MAX) * 2 - 1;
		float y = ((float)rand() / (float)RAND_MAX) * 2 - 1;
		float z = ((float)rand() / (float)RAND_MAX) * 2 - 1;
		vec3 randomAxis = glm::normalize(vec3(x, y, z));
		float from = ((float)rand() / (float)RAND_MAX) * 2 - 1;
		float to = ((float)rand() / (float)RAND_MAX) * 2 - 1;
		if (from > to) {
			float t = to;
			to = from;
			from = t;
		}
		test_tree.AxisRange(randomAxis, from, to, r1);
		baseAR(randomAxis, from, to, b1);
		if (r1.size() == b1.size()) {
			sort(r1.begin(), r1.end());
			sort(b1.begin(), b1.end());
			for (int j = 0; j < r1.size(); j++) {
				if (!r1[j] == b1[j]) {
					pass = false;
					cout << "**ERROR**" << endl;
					break;
				}
			}
		}
		else {
			cout << "**ERROR**" << endl;
			pass = false;
		}
	}
	if (pass) {
		cout << "All tests passed!" << endl;
	}
	else {
		cout << "Something is wrong!" << endl;
	}
	cout << "DONE" << endl;
}

void KdTreeTest::benchmarkNN() 
{
	cout << endl << endl << "::NEAREST NEIGHBOUR BENCHMARK::" << endl << endl;
	float averageTime1 = 0, averageTime2 = 0;
	float averageNodes1 = 0, averageNodes2 = 0;
	high_resolution_clock::time_point begin;
	high_resolution_clock::time_point end;
	duration<double, std::milli> time_span;

	for (uint32_t i = 0; i < BENCHMARK_REPS; i++) {
		float x = (float)rand() / (float)RAND_MAX;
		float y = (float)rand() / (float)RAND_MAX;
		float z = (float)rand() / (float)RAND_MAX;
		vec3 randomPos(x, y, z);
		uint32_t randomInd = rand() % BENCHMARK_COUNT;
		uint32_t r1, r2;
		
		begin = high_resolution_clock::now();
		benchmark_tree.NearestSearch(randomInd, r1);
		end = high_resolution_clock::now();
		time_span = end - begin;
		averageTime1 += time_span.count();
		averageNodes1 += benchmark_tree._expandedNodes;

		begin = high_resolution_clock::now();
		benchmark_tree.NearestSearch(randomPos, r2);
		end = high_resolution_clock::now();
		time_span = end - begin;
		averageTime2 += time_span.count();
		averageNodes2 += benchmark_tree._expandedNodes;
	}

	averageTime1 /= BENCHMARK_REPS;
	averageTime2 /= BENCHMARK_REPS;
	averageNodes1 /= BENCHMARK_REPS;
	averageNodes2 /= BENCHMARK_REPS;

	cout << "FROM ANY POINT SEARCH:" << endl;
	cout << "  -Average time: " << averageTime2 << endl;
	cout << "  -Average expanded nodes: " << averageNodes2 << endl;
	cout << endl;
	
	cout << "FROM MODEL POINT SEARCH:" << endl;
	cout << "  -Average time: " << averageTime1 << endl;
	cout << "  -Average expanded nodes: " << averageNodes1 << endl;
	cout << "  -Estimated nearest search for all model points: " << averageTime1*BENCHMARK_COUNT << endl;
	cout << endl;
}



void KdTreeTest::baseNN(uint32_t index, uint32_t&result) 
{
	float minDist = std::numeric_limits<float>().max();
	uint32_t best = 0;
	vec3 pos = test_model[index].position;
	for (uint32_t j = 0; j < TEST_COUNT; j++) {
		float d = glm::distance(test_model[j].position, pos);
		if (j != index && d < minDist) {
			best = j;
			minDist = d;
		}
	}
	result = best;
}

void KdTreeTest::baseNN(vec3 pos, uint32_t& result)
{
	float minDist = std::numeric_limits<float>().max();
	uint32_t best = 0;
	for (uint32_t j = 0; j < TEST_COUNT; j++) {
		float d = glm::distance(test_model[j].position, pos);
		if (d < minDist) {
			best = j;
			minDist = d;
		}
	}
	result = best;
}

void KdTreeTest::baseKNN(uint32_t index, unsigned int k, std::vector<uint32_t>& result)
{
	result.clear();
	vec3 pos = test_model[index].position;
	struct Compare {
		Point* model;
		vec3 pos;
        Compare(Point* m, vec3 p) : model(m), pos(p){}
		bool operator() (uint32_t v1, uint32_t v2){
			return glm::distance(model[v1].position, pos) < glm::distance(model[v2].position, pos);
		}
	};

	Compare comp(test_model, pos);
	
	for (uint32_t i = 0; i < TEST_COUNT; i++) {
		float d = glm::distance(test_model[i].position, pos);
		if (i != index) {
			if (result.size() < k) {
				result.push_back(i);
				push_heap(result.begin(), result.end(), comp);
			}
			else {
				uint32_t worstK = result.front();
				if (d < glm::distance(test_model[worstK].position, pos)) {
					pop_heap(result.begin(), result.end(), comp);
					result[k - 1] = i;
					push_heap(result.begin(), result.end(), comp);
				}
			}
		}
		
	}
	sort_heap(result.begin(), result.end(), comp);
}

void KdTreeTest::baseKNN(vec3 pos, unsigned int k, std::vector<uint32_t>& result)
{
	result.clear();
	struct Compare {
		Point* model;
		vec3 pos;
		Compare(Point* m, vec3 p) : model(m), pos(p) {}
		bool operator() (uint32_t v1, uint32_t v2) {
			return glm::distance(model[v1].position, pos) < glm::distance(model[v2].position, pos);
		}
	};

	Compare comp(test_model, pos);

	for (uint32_t i = 0; i < TEST_COUNT; i++) {
		float d = glm::distance(test_model[i].position, pos);
		if (result.size() < k) {
			result.push_back(i);
			push_heap(result.begin(), result.end(), comp);
		}
		else {
			uint32_t worstK = result.front();
			if (d < glm::distance(test_model[worstK].position, pos)) {
				pop_heap(result.begin(), result.end(), comp);
				result[k - 1] = i;
				push_heap(result.begin(), result.end(), comp);
			}
		}
	}
	sort_heap(result.begin(), result.end(), comp);
}

void KdTreeTest::baseMin(glm::vec3 axis, uint32_t& result, float& value)
{
	axis = glm::normalize(axis);
	float minDist = std::numeric_limits<float>().max();
	uint32_t best = 0;
	for (uint32_t j = 0; j < TEST_COUNT; j++) {
		float d = glm::dot(test_model[j].position, axis);
		if (d < minDist) {
			best = j;
			minDist = d;
		}
	}
	result = best;
	value = minDist;
}

void KdTreeTest::baseMax(glm::vec3 axis, uint32_t& result, float& value)
{
	axis = glm::normalize(axis);
	float maxDist = -std::numeric_limits<float>().max();
	uint32_t best = -1;
	for (uint32_t j = 0; j < TEST_COUNT; j++) {
		float d = glm::dot(test_model[j].position, axis);
		if (d > maxDist) {
			best = j;
			maxDist = d;
		}
	}
	result = best;
	value = maxDist;
}

void KdTreeTest::baseAR(vec3 axis, float from, float to, vector<uint32_t> &result) 
{
	result.clear();
	axis = glm::normalize(axis);
	for (uint32_t i = 0; i < TEST_COUNT; i++) {
		float d = glm::dot(test_model[i].position, axis);
		if (d >= from && d < to) {
			result.push_back(i);
		}
	}
}

