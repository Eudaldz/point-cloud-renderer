#include "tests/test.h"
#include "tests/kdtree_test.h"

Test* Test::NewTest(const char* id)
 {
	 if (strcmp(id, "kdtree") == 0) {
		 return new KdTreeTest();
	 }
	 return nullptr;
}