#include "point_cloud.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
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
		
		
		
		
		
	}

	PointCloud readPCD(istream& is)
	{
		string line;
		istringstream ls;
		string param;
		vector<string> fields;
		vector<int> sizes;
		vector<string> types;
		int pn;
		string dataFormat;
		bool endOfConfig = false;
		//READ CONFIG PARAMATERS
		while (!endOfConfig && getline(is, line)) {
			ls.str(line);
			ls >> param;
			if (param == "FIELDS") {
				string v;
				while (ls >> v) {
					fields.push_back(v);
				}
			} else if (param == "SIZE") {
				int v;
				while (ls >> v) {
					sizes.push_back(v);
				}
			} else if (param == "TYPE") {
				string v;
				while (ls >> v) {
					types.push_back(v);
				}
			} else if (param == "POINTS") {
				ls >> pn;
			} else if (param == "DATA") {
				ls >> dataFormat;
				endOfConfig = true;
			}
		}
		//READ DATA
		PointCloud result;
		result.vn = pn;
		result.points = new Point[pn];
		if (dataFormat == "ascii") {
			for (int i = 0; i < pn; i++) {
				Point p;
				if (getline(is, line)) {
					ls.str(line);
					float32_t f;
					string id;
					for (int j = 0; j < fields.size(); j++) {
						id = fields[j];
						if (ls >> f) {
							setField(id, p, f);
							result.points[i] = p;
						} else {
							throw "Bad file format";
						}
					}
				} else {
					throw "Bad file format";
				}
			}
		} else if (dataFormat == "binary") {
			for (int i = 0; i < pn; i++) {
				Point p;
				for (int j = 0; j < fields.size(); j++) {
					if (!is.good()) {
						throw "Bad file format";
					}
					if (sizes[j] == 4) {
						uint32_t word = 0;
						uint8_t byte0 = is.get();
						uint8_t byte1 = is.get();
						uint8_t byte2 = is.get();
						uint8_t byte3 = is.get();
						word = byte3*256*256*256 + byte2*256*256 + byte1*256 +byte0;
						float f;
						if (types[j] == "F") {
							f = *(float*)&word;
						} else if (types[j] == "I") {
							f = (float)word;
						} else {
							"Currently unsupported " + types[j] + " type field";
						}
						string id = fields[j];
						setField(id, p, f);
						result.points[i] = p;

					} else {
						throw "Currently unsupported " + to_string(sizes[j]) + " byte field";
					}
				}
			}
		} else {
			throw "Bad file format";
		}
		return result;
	}

	void setField(string fieldId, Point& p, float f)
	{
		if (fieldId == "x") {
			p.position.x = f;
		} else if (fieldId == "y") {
			p.position.y = f;
		} else if (fieldId == "z") {
			p.position.z = f;
		} else if (fieldId == "rgb") {
			uint32_t rgb = *(uint32_t*)&f;
			uint8_t r = (rgb >> 16) & 0x0000ff;
			uint8_t g = (rgb >> 8) & 0x0000ff;
			uint8_t b = (rgb) & 0x0000ff;
			p.color.r = (float)r / 255.0f;
			p.color.g = (float)g / 255.0f;
			p.color.b = (float)b / 255.0f;
			p.color.a = 1.0f;
		} else if (fieldId == "rgba") {
			uint32_t rgb = *(uint32_t*)&f;
			uint8_t a = (rgb >> 24) & 0x0000ff;
			uint8_t r = (rgb >> 16) & 0x0000ff;
			uint8_t g = (rgb >> 8) & 0x0000ff;
			uint8_t b = (rgb) & 0x0000ff;
			p.color.r = (float)r / 255.0f;
			p.color.g = (float)g / 255.0f;
			p.color.b = (float)b / 255.0f;
			p.color.a = (float)a / 255.0f;
		} else if (fieldId == "normal_x") {
			p.normal.x = f;
		} else if (fieldId == "normal_y") {
			p.normal.y = f;
		} else if (fieldId == "normal_z") {
			p.normal.z = f;
		}
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




