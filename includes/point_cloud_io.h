#pragma once
#include "point_cloud.h"
#include <string>

namespace PCIO {
	struct Options {

	};
	
	PointCloud* readPointCloud(const std::string &filename, const Options &params);
}