#pragma once
#include "scene.h"
#include "input_test.h"
#include "cube_test.h"
#include "camera_test.h"

namespace DemoCollection {
	Scene* OpenDemo(const char* id)
	{
		if (strcmp(id, "input") == 0) {
			return new InputTest();
		} else if (strcmp(id, "cube") == 0) {
			return new CubeTest();
		} else if (strcmp(id, "camera") == 0) {
			return new CameraTest();
		}
		return NULL;
	}
}