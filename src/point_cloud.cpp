#include "point_cloud.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace glm;

namespace
{
	PointCloud readPCD(istream& is);
	void setField(string fieldId, Point& p, float32_t f);
	void setField(string fieldId, Point& p, uint32_t f);
	float minDist(int i, PointCloud pc);
	float estimatePointSize(PointCloud pc);
	vec3 getCenter(PointCloud pc);
	float getRadius(PointCloud pc);
	
	vec4 white_color_map(vec3 coord)
	{
		return vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	
	PointCloud parseFromStream(istream& is)
	{
		string line;
		istringstream ls;
		string word;
		if (getline(is, line)) {
			ls.str(line);
			char c = ls.get();
			if (c == '#') {
				ls >> word;
				if (word == ".PCD") {
					return readPCD(is);
				} else {
					throw "Unrecognized file format";
				}
			} else {
				throw "Unrecognized file format";
			}
		} else {
			throw "Bad file format";
		}
	}

	PointCloud readPCD(istream& is)
	{
		string line;
		string param;
		vector<string> fields;
		vector<int> sizes;
		vector<string> types;
		int pn = 0;
		string dataFormat;
		bool endOfConfig = false;
		//READ CONFIG PARAMATERS
		while (!endOfConfig && getline(is, line)) {
			istringstream ls(line);
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
		if (pn == 0) {
			throw "Bad file format";
		}
		PointCloud result;
		result.vn = pn;
		result.points = new Point[pn];
		if (dataFormat == "ascii") {
			for (int i = 0; i < pn; i++) {
				Point p;
				if (getline(is, line)) {
					istringstream ls(line);
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
				if (!is.good()) {
					throw "Bad file format";
				}
				Point p;
				for (int j = 0; j < fields.size(); j++) {
					if (sizes[j] == 4) {
						uint32_t word = 0;
						uint8_t byte0 = is.get();
						uint8_t byte1 = is.get();
						uint8_t byte2 = is.get();
						uint8_t byte3 = is.get();
						word = byte3*256*256*256 + byte2*256*256 + byte1*256 +byte0;
						string id = fields[j];
						setField(id, p, word);
					} else {
						throw "Currently unsupported " + to_string(sizes[j]) + " byte field";
					}
				}
				result.points[i] = p;
			}
		} else {
			throw "Bad file format";
		}
		return result;
	}

	void setField(string fieldId, Point& p, uint32_t v)
	{
		float f = *(float*)&v;
		if (fieldId == "x") {
			p.position.x = f;
		} else if (fieldId == "y") {
			p.position.y = f;
		} else if (fieldId == "z") {
			p.position.z = f;
		} else if (fieldId == "rgb") {
			uint32_t rgb = v;
			uint8_t r = (rgb >> 16) & 0x0000ff;
			uint8_t g = (rgb >> 8) & 0x0000ff;
			uint8_t b = (rgb) & 0x0000ff;
			p.color.r = (float)r / 255.0f;
			p.color.g = (float)g / 255.0f;
			p.color.b = (float)b / 255.0f;
			p.color.a = 1.0f;
		} else if (fieldId == "rgba") {
			uint32_t rgb = v;
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

	void setField(string fieldId, Point& p, float32_t f)
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

	float estimatePointSize(PointCloud pc)
	{
		if (pc.vn == 0)return 0;
		vector<float> bestSize = vector<float>(pc.vn);
		vector<float> filtered = vector<float>();
		filtered.reserve(pc.vn);
		float sum = 0;
		float sum2 = 0;
		for (int i = 0; i < pc.vn; i++) {
			float d = minDist(i, pc);
			bestSize[i] = d;
			sum += d;
			sum2 += d * d;
		}
		float mean = sum / pc.vn;
		float std_dev = glm::sqrt((sum2 - pc.vn * mean * mean) / (pc.vn - 1));
		float filteredSum = 0;
		size_t filteredN = 0;
		float threshold = 3;
		for (int i = 0; i < pc.vn; i++) {
			float zscore = glm::abs((bestSize[i] - mean) / std_dev);
			if (zscore < threshold) {
				filteredN += 1;
				filteredSum += bestSize[i];
				filtered.push_back(bestSize[i]);
			}
		}
		if (filteredN == 0) {
			return 0;
		}
		float result1 = filteredSum / filteredN;
		
		sort(filtered.begin(), filtered.end());
		vector<float> intervals = vector<float>(5);
		vector<int> counts = vector<int>(5);
		float min = filtered[0];
		float max = filtered[filtered.size() - 1];
		float step = (max - min) / 5.0f;
		for (int i = 0; i < 5; i++) {
			intervals[i] = min + step * (i+1);
		}
		for (int i = 0; i < filtered.size(); i++) {
			float f = filtered[i];
			if (f < intervals[0])counts[0]++;
			else if (f < intervals[1])counts[1]++;
			else if (f < intervals[2])counts[2]++;
			else if (f < intervals[3])counts[3]++;
			else counts[4]++;
		}
		
		cout << "min: " << filtered[0] << ", max: " << filtered[filtered.size()-1] << "\n";
		float result2 = filtered[filtered.size() / 2];
		cout << "result1: " << result1 << ", result2: " << result2 << "\n";
		for (int i = 0; i < 5; i++) {
			cout << " int: " << (i==0 ? min : intervals[i-1]) << ", "<<intervals[i] << "count: " << counts[i] << endl;
		}
		return result1;

	}

	float minDist(int idx, PointCloud pc)
	{
		vec3 pos = pc.points[idx].position;
		float min = std::numeric_limits<float>::max();
		for (int i = 0; i < pc.vn; i++) {
			vec3 pos2 = pc.points[i].position;
			if (i != idx) {
				float dist = glm::distance(pos2, pos);
				min = dist < min ? dist : min;
			}
		}
		return min;
	}

	vec3 getCenter(PointCloud pc)
	{
		vec3 c = vec3(0,0,0);
		for (size_t i = 0; i < pc.vn; i++) {
			c += pc.points[i].position;
		}
		return c / (float)pc.vn;
	}

	float getRadius(PointCloud pc)
	{
		float r = 0;
		for (size_t i = 0; i < pc.vn; i++) {
			float tr = glm::distance(pc.center, pc.points[i].position);
			if (r < tr) r = tr;
		}
		return r;
	}
}

PointCloud PCReader::parseString(const char* str)
{
	istringstream iss(str);
	PointCloud result = parseFromStream(iss);
	result.estimatedPointSize = estimatePointSize(result);
	result.center = getCenter(result);
	result.radius = getRadius(result);
	return result;
}

PointCloud PCReader::parseFile(const char* fn)
{
	string p = string(fn);
	std::filesystem::path cwd = std::filesystem::current_path() / p;
	ifstream ifs;
	PointCloud result;
	result.points = nullptr;
	result.vn = 0;
	//try {
		ifs.open(fn);
		if (ifs.good()) {
			result = parseFromStream(ifs);
		} else {
			cout << "ERROR: Point cloud file does not exist" << endl;
		}
		ifs.close();
	//} catch (...) {
		//cout << "ERROR: Point cloud file could not be read" << endl;
	//}
	result.estimatedPointSize = estimatePointSize(result);
	result.center = getCenter(result);
	result.radius = getRadius(result);
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
				size_t ind = i * sampleRes * sampleRes + j * sampleRes + k;
				float xoff = -1.0f + (float)i / (float)(sampleRes - 1) * 2.0f;
				float yoff = -1.0f + (float)j / (float)(sampleRes - 1) * 2.0f;
				float zoff = -1.0f + (float)k / (float)(sampleRes - 1) * 2.0f;
				vec3 pos = vec3(xoff, yoff, zoff);
				vec4 color = f(pos);
				result.points[ind] = Point(pos, color);
			}
		}
	}
	return result;
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




