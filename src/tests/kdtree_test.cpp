#include "tests/kdtree_test.h"
#include <iostream>
#include <glm/glm.hpp>
#include <chrono>

using namespace std;
using namespace glm;
using namespace std::chrono;

KdTreeTest::KdTreeTest() 
{

}

void KdTreeTest::Run() {
	cout << "\nEnter number of points: ";
	cin >> size;
	generateModel();
	tree.model = model;
	tree.size = size;
	tree.Construct();
	testAR();
}

void KdTreeTest::generateModel() {
	model = new Point[size];
	std::srand(std::time(nullptr));
	cout << endl << "MODEL:" << endl;
	for (uint32_t i = 0; i < size; i++) {
		float x = (float)rand() / (float)RAND_MAX;
		float y = (float)rand() / (float)RAND_MAX;
		float z = (float)rand() / (float)RAND_MAX;
		model[i].position = vec3(x, y, z);
		cout << i <<": ( " << x << " " << y << " " << z << " )" << endl;
	}
	cout << endl;
}
void KdTreeTest::testAR() 
{
	vec3 axis((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX);
	axis = axis * 2.0f - vec3(1, 1, 1);
	axis = glm::normalize(axis);
	cout << endl << "AXIS RANGE TEST: (" << axis.x <<"," << axis.y <<"," << axis.z<< endl;
	uint32_t ip;
	float minP, maxP;
	tree.MinAxis(axis, ip, minP);
	tree.MaxAxis(axis, ip, maxP);
	cout << endl << "MINP: " << minP << endl << "MAXP: " << maxP << endl;
	float depthStep = 0.1f;
	float currentDepth = minP - depthStep / 2.0f;
	float endDepth = maxP + 2.0f * std::numeric_limits<float>().epsilon();
	vector<uint32_t> buffer;
	vector<uint32_t> baseB;

	do {
		buffer.clear();
		baseB.clear();
		tree.RangeAxis(axis, currentDepth, currentDepth + depthStep, buffer);
		baseAxisRange(axis, currentDepth, currentDepth + depthStep, baseB);
		cout << endl << "RANGE [ " << currentDepth << " , " << currentDepth + depthStep << " ) ::"<< buffer.size() << endl;
		cout << "TREE RESULT" << endl;
		for (int j = 0; j < buffer.size(); j++) {
			cout << buffer[j] << endl;
		}
		cout << "BASE RESULT" << endl;
		for (int j = 0; j < baseB.size(); j++) {
			cout << baseB[j] << endl;
		}
		currentDepth += depthStep;
	} while (currentDepth < endDepth);
}

void KdTreeTest::testNN() 
{
	uint32_t bnn;
	uint32_t tnn;
	Point p;
	uint32_t avgExpNodes = 0;
	uint32_t expN = 0;
	cout << endl <<"NEAREST NEIGHBOUR PRIORITY QUEUE:" << endl;
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	for (size_t i = 0; i < size; i++) {
		tree.NearestSearchBenchmark1(i, p, expN);
		avgExpNodes += expN;
	}
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration<double, std::milli> time_span = t2 - t1;
	cout << endl << "TIME: " << time_span.count();
	cout << endl << "AVERAGE EXPANDED NODES: " << double(avgExpNodes) / (double)size << endl;
	avgExpNodes = 0;
	expN = 0;
	t1 = high_resolution_clock::now();
	cout << endl <<"TEST NEAREST NEIGHBOUR QUEUE:" << endl;
	for (size_t i = 0; i < size; i++) {
		tree.NearestSearchBenchmark2(i, p, expN);
		avgExpNodes += expN;
	}
	t2 = high_resolution_clock::now();
	time_span = t2 - t1;
	cout << endl << "TIME: " << time_span.count();
	cout << endl << "AVERAGE EXPANDED NODES: " << double(avgExpNodes) / (double)size;
}

uint32_t KdTreeTest::baseNN(uint32_t i) {
	float minDist = std::numeric_limits<float>().max();
	uint32_t best = 0;
	vec3 pos = model[i].position;
	for (uint32_t j = 0; j < size; j++) {
		float d = glm::distance(model[j].position, pos);
		if (j != i && d < minDist) {
			best = j;
			minDist = d;
		}
	}
	return best;
}

void KdTreeTest::baseAxisRange(vec3 axis, float minP, float maxP, vector<uint32_t> &buff) 
{
	axis = glm::normalize(axis);
	for (uint32_t i = 0; i < size; i++) {
		float d = glm::dot(model[i].position, axis);
		if (d >= minP && d < maxP) {
			buff.push_back(i);
		}
	}
}

