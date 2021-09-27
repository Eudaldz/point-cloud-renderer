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
	testNN();
}

void KdTreeTest::generateModel() {
	model = new Point[size];
	std::srand(std::time(nullptr));
	cout << endl << "MODEL:" << endl;
	for (size_t i = 0; i < size; i++) {
		float x = (float)rand() / (float)RAND_MAX;
		float y = (float)rand() / (float)RAND_MAX;
		float z = (float)rand() / (float)RAND_MAX;
		model[i].position = vec3(x, y, z);
		//cout << i <<": ( " << x << " " << y << " " << z << " )" << endl;
	}
	cout << endl;
}

void KdTreeTest::testNN() 
{
	size_t bnn;
	size_t tnn;
	Point p;
	size_t avgExpNodes = 0;
	size_t expN = 0;
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

size_t KdTreeTest::baseNN(size_t i) {
	float minDist = std::numeric_limits<float>().max();
	size_t best = 0;
	vec3 pos = model[i].position;
	for (size_t j = 0; j < size; j++) {
		float d = glm::distance(model[j].position, pos);
		if (j != i && d < minDist) {
			best = j;
			minDist = d;
		}
	}
	return best;
}

