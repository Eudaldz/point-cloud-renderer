#include "tests/queue_test.h"
#include <iostream>
#include "search_queue.h"
#include <queue>
#include <chrono>

using namespace std;
using namespace std::chrono;

QueueTest::QueueTest() {}

void QueueTest::benchmarkNormalQueue()
{
	cout << endl << ":: NORMAL QUEUE BENCHMARK ::" << endl << endl;
	cout << "...generating dataset...";
	
	int* dataset = new int[TESTN_COUNT];
	for (size_t i = 0; i < TESTN_COUNT; i++) {
		dataset[i] = rand();
	}
	cout << "DONE" << endl << endl;
	NormalQueue<int> nqueue;
	queue<int> queue;
	high_resolution_clock::time_point begin;
	high_resolution_clock::time_point end;
	duration<double, std::milli> time_span;
	cout << ":: PHASE 1 [CONTINUOUS PUSH] ::" << endl;
	cout << "* Custom Queue:" << endl;
	begin = high_resolution_clock::now();
	for (size_t i = 0; i < TESTN_COUNT; i++) {
		nqueue.Push(dataset[i]);
	}
	for (size_t i = 0; i < TESTN_COUNT; i++) {
		nqueue.Pop();
	}
	end = high_resolution_clock::now();
	time_span = end - begin;
	cout << "TIME: " << time_span.count() << endl;

	cout << "* Standard Queue:" << endl;
	begin = high_resolution_clock::now();
	for (size_t i = 0; i < TESTN_COUNT; i++) {
		queue.push(dataset[i]);
	}
	for (size_t i = 0; i < TESTN_COUNT; i++) {
		queue.pop();
	}
	end = high_resolution_clock::now();
	time_span = end - begin;
	cout << "TIME: " << time_span.count() << endl;
	cout << ":: PHASE 2 [MIX PUSH/POP] ::" << endl;
	cout << "* Custom Queue:" << endl;
	begin = high_resolution_clock::now();
	for (size_t i = 0; i < TESTN_COUNT; i++) {
		nqueue.Push(dataset[i]);
		nqueue.Push(dataset[i] - 1);
		nqueue.Push(dataset[i] + 1);
		nqueue.Pop();
		nqueue.Pop();
	}
	end = high_resolution_clock::now();
	time_span = end - begin;
	cout << "TIME: " << time_span.count() << endl;
	cout << "* Standard Queue:" << endl;
	begin = high_resolution_clock::now();
	for (size_t i = 0; i < TESTN_COUNT; i++) {
		queue.push(dataset[i]);
		queue.push(dataset[i] - 1);
		queue.push(dataset[i] + 1);
		queue.pop();
		queue.pop();
	}
	end = high_resolution_clock::now();
	time_span = end - begin;
	cout << "TIME: " << time_span.count() << endl;
}


void QueueTest::testNormalQueue()
{
	cout << endl << ":: NORMAL QUEUE TEST ::" << endl << endl;
	cout << "...testing..." << endl;
	NormalQueue<int> nqueue;
	queue<int> queue;
	bool error = false;
	for (int i = 0; i < TESTN_COUNT; i++) {
		int v = rand();
		float action = (float)rand() / (float)RAND_MAX;
		if (nqueue.Size() != queue.size()) {
			error = true;
			cout << "**ERROR**" << endl;
			break;
		}
		
		if (action > 0.25f || nqueue.Empty()) {
			nqueue.Push(v);
			queue.push(v);
		}
		else {
			if (nqueue.Front() != queue.front()) {
				error = true;
				cout << "**ERROR**" << endl;
			}
			nqueue.Pop();
			queue.pop();
		}
	}
	while (!nqueue.Empty() && !queue.empty()) {
		if (nqueue.Front() != queue.front()) {
			error = true;
			cout << "**ERROR**" << endl;
		}
		nqueue.Pop();
		queue.pop();
	}
	if (nqueue.Size() != queue.size()) {
		error = true;
		cout << "**ERROR**" << endl;
	}

	if (!error) {
		cout << endl << "ALL TESTS PASSED!!" << endl;
	}
	else {
		cout << endl << "SOMETHING WENT WRONG!!" << endl;
	}
}

void QueueTest::testPriorityQueue() 
{
	cout << endl << ":: PRIORITY QUEUE TEST ::" << endl << endl;
	cout << "...testing..." << endl;
	struct bigger {
		bool operator() (int x, int y) { return x >= y; }
	};
	
	PriorityQueue<int> nqueue;
	priority_queue<int> queue;
	bool error = false;
	for (int i = 0; i < TESTP_COUNT; i++) {
		int v = rand();
		float action = (float)rand() / (float)RAND_MAX;
		if (nqueue.Size() != queue.size()) {
			error = true;
			cout << "**ERROR**" << endl;
			break;
		}

		if (action > 0.25f || nqueue.Empty()) {
			nqueue.Push(v);
			queue.push(v);
		}
		else {
			if (nqueue.Front() != queue.top()) {
				error = true;
				cout << "**ERROR**" << endl;
			}
			nqueue.Pop();
			queue.pop();
		}
	}
	while (!nqueue.Empty() && !queue.empty()) {
		if (nqueue.Front() != queue.top()) {
			error = true;
			cout << "**ERROR**" << endl;
		}
		nqueue.Pop();
		queue.pop();
	}
	if (nqueue.Size() != queue.size()) {
		error = true;
		cout << "**ERROR**" << endl;
	}

	if (!error) {
		cout << endl << "ALL TESTS PASSED!!" << endl;
	}
	else {
		cout << endl << "SOMETHING WENT WRONG!!" << endl;
	}
}

void QueueTest::benchmarkPriorityQueue()
{
	cout << endl << ":: PRIORITY QUEUE BENCHMARK ::" << endl << endl;
	cout << "...generating dataset...";

	int* dataset = new int[TESTP_COUNT];
	for (size_t i = 0; i < TESTP_COUNT; i++) {
		dataset[i] = rand();
	}
	cout << "DONE" << endl << endl;
	PriorityQueue<int> nqueue;
	priority_queue<int> queue;
	high_resolution_clock::time_point begin;
	high_resolution_clock::time_point end;
	duration<double, std::milli> time_span;
	cout << ":: PHASE 1 [CONTINUOUS PUSH] ::" << endl;
	cout << "* Custom Queue:" << endl;
	begin = high_resolution_clock::now();
	for (size_t i = 0; i < TESTP_COUNT; i++) {
		nqueue.Push(dataset[i]);
	}
	end = high_resolution_clock::now();
	time_span = end - begin;
	cout << "TIME: " << time_span.count() << endl;
	begin = high_resolution_clock::now();
	for (size_t i = 0; i < TESTP_COUNT; i++) {
		nqueue.Pop();
	}
	end = high_resolution_clock::now();
	time_span = end - begin;
	cout << "TIME: " << time_span.count() << endl;

	cout << "* Standard Queue:" << endl;
	begin = high_resolution_clock::now();
	for (size_t i = 0; i < TESTP_COUNT; i++) {
		queue.push(dataset[i]);
	}
	end = high_resolution_clock::now();
	time_span = end - begin;
	cout << "TIME: " << time_span.count() << endl;
	begin = high_resolution_clock::now();
	for (size_t i = 0; i < TESTP_COUNT; i++) {
		queue.pop();
	}
	end = high_resolution_clock::now();
	time_span = end - begin;
	cout << "TIME: " << time_span.count() << endl;
	cout << ":: PHASE 2 [MIX PUSH/POP] ::" << endl;
	cout << "* Custom Queue:" << endl;
	begin = high_resolution_clock::now();
	for (size_t i = 0; i < TESTP_COUNT; i++) {
		nqueue.Push(dataset[i]);
		nqueue.Push(dataset[i] - 1);
		nqueue.Push(dataset[i] + 1);
		nqueue.Pop();
		nqueue.Pop();
	}
	end = high_resolution_clock::now();
	time_span = end - begin;
	cout << "TIME: " << time_span.count() << endl;
	cout << "* Standard Queue:" << endl;
	begin = high_resolution_clock::now();
	for (size_t i = 0; i < TESTP_COUNT; i++) {
		queue.push(dataset[i]);
		queue.push(dataset[i] - 1);
		queue.push(dataset[i] + 1);
		queue.pop();
		queue.pop();
	}
	end = high_resolution_clock::now();
	time_span = end - begin;
	cout << "TIME: " << time_span.count() << endl;
}


void QueueTest::Run() 
{
	std::srand((unsigned int)std::time(nullptr));
	testNormalQueue();
	benchmarkNormalQueue();
	testPriorityQueue();
	benchmarkPriorityQueue();
}