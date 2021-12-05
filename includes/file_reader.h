#pragma once
#include <string>
#include <vector>
#include "point.h"


namespace Reader {
	struct Options {

	};
	
	int ReadPointCloud(const std::string &filename, const Options &params, std::vector<Point>& points);
}