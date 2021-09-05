#include "point_cloud.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;
using namespace glm;

namespace
{
	vec4 white_color_map(vec3 coord)
	{
		return vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	
	PointCloud parseFromStream(istream& is)
	{
		float v;
		vector<float> data;
		PointCloud result;
		result.vn = 0;
		result.points = nullptr;
		while (is >> v) {

		}
		return result;
	}
}

PointCloud PCReader::parseString(const char* str)
{
	istringstream iss(str);
	PointCloud result = parseFromStream(iss);
	return result;
}

PointCloud PCReader::parseFile(const char* fn)
{
	ifstream ifs;
	PointCloud result;
	result.points = nullptr;
	result.vn = 0;
	try {
		ifs.open(fn);
		result = parseFromStream(ifs);
		ifs.close();
	} catch (ifstream::failure& e) {
		cout << "ERROR: Point cloud file could not be read" << endl;
	}
	return result;
}


PointCloud PCPrimitives::square(size_t sampleRes)
{
	return square(&white_color_map, sampleRes);
}

PointCloud PCPrimitives::square(color_map* f, size_t sampleRes)
{
	assert(sampleRes > 0);
	if (sampleRes > MAX_SAMPLE_RES)sampleRes = MAX_SAMPLE_RES;
	size_t vn = sampleRes * sampleRes;
	PointCloud result;
	result.vn = vn;
	result.points = new Point[vn];
	
	for (size_t i = 0; i < sampleRes; i++) {
		for (size_t j = 0; j < sampleRes; j++) {
			size_t ind = (i * sampleRes + j);
			float xoff = -1.0f + (float)i / (float)(sampleRes - 1) * 2.0f;
			float yoff = -1.0f + (float)j / (float)(sampleRes - 1) * 2.0f;
			vec3 pos = vec3(xoff, yoff, 0);
			vec4 color = f(pos);
			result.points[ind] = Point(pos, color);
		}
	}
	return result;
}

PointCloud PCPrimitives::cube(size_t sampleRes)
{
	return cube(&white_color_map, sampleRes);
}

PointCloud PCPrimitives::cube(color_map* f, size_t sampleRes)
{
	assert(sampleRes > 0);
	if (sampleRes > MAX_SAMPLE_RES)sampleRes = MAX_SAMPLE_RES;
	size_t vn = sampleRes * sampleRes * sampleRes;
	PointCloud result;
	result.vn = vn;
	result.points = new Point[vn];

	for (size_t i = 0; i < sampleRes; i++) {
		for (size_t j = 0; j < sampleRes; j++) {
			for (size_t k = 0; k < sampleRes; k++) {
				size_t ind =  i * sampleRes * sampleRes + j * sampleRes + k;
				float xoff = -1.0f + (float)i / (float)(sampleRes - 1) * 2.0f;
				float yoff = -1.0f + (float)j / (float)(sampleRes - 1) * 2.0f;
				float zoff = -1.0f + (float)k / (float)(sampleRes - 1) * 2.0f;
				vec3 pos = vec3(xoff, yoff, zoff);
				vec4 color = f(pos);
				result.points[ind] = Point(pos, color);
			}
		}
	}
}

PointCloud PCPrimitives::sphere(size_t sampleRes)
{
	return sphere(&white_color_map, sampleRes);
}

PointCloud PCPrimitives::sphere(color_map* f, size_t sampleRes)
{
	assert(sampleRes > 0);
	if (sampleRes > MAX_SAMPLE_RES)sampleRes = MAX_SAMPLE_RES;
	size_t vn = sampleRes * sampleRes * sampleRes;
	vector<Point> buffer;
	buffer.reserve(vn);

	for (size_t i = 0; i < sampleRes; i++) {
		for (size_t j = 0; j < sampleRes; j++) {
			for (size_t k = 0; k < sampleRes; k++) {
				size_t ind = i * sampleRes * sampleRes + j * sampleRes + k;
				float xoff = -1.0f + (float)i / (float)(sampleRes - 1) * 2.0f;
				float yoff = -1.0f + (float)j / (float)(sampleRes - 1) * 2.0f;
				float zoff = -1.0f + (float)k / (float)(sampleRes - 1) * 2.0f;
				vec3 pos = vec3(xoff, yoff, zoff);
				vec4 color = f(pos);
				if (glm::length(pos) <= 1) {
					buffer.push_back(Point(pos, color));
				}
			}
		}
	}
	PointCloud result;
	result.vn = buffer.size();
	result.points = new Point[result.vn];
	std::copy(buffer.begin(), buffer.end(), result.points);
	return result;
}




