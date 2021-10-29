#include "tests/test.h"
#include "tests/kdtree_test.h"
#include "tests/queue_test.h"

Test* Test::NewTest(const char* id)
 {
	 if (strcmp(id, "kdtree") == 0) {
		 return new KdTreeTest();
	 }
	 if (strcmp(id, "queue") == 0) {
		 return new QueueTest();
	 }
	 return nullptr;
}