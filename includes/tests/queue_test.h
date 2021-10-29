#pragma once
#include "test.h"

class QueueTest : public Test 
{
private:
	static constexpr size_t TESTN_COUNT = 100000000;
	static constexpr size_t TESTP_COUNT = 100000000;
	
	void benchmarkNormalQueue();
	void testNormalQueue();
	void testPriorityQueue();
	void benchmarkPriorityQueue();
public:
	QueueTest();
	void Run();
};