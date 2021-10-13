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
	PointCloud* readPCD(istream& is);
	void setField(string fieldId, Point& p, float32_t f);
	void setField(string fieldId, Point& p, uint32_t f);
	float minDist(int i, PointCloud pc);
	float estimatePointSize(PointCloud pc);
	vec3 getCenter(PointCloud pc);
	float getRadius(PointCloud pc);
	
	float side(vec2 p1, vec2 p2, vec2 c)
	{
		vec2 dir = p2 - p1;
		vec2 v = c - p1;
		return v.x * dir.y - v.y * dir.x;
	}
	
	bool inside_triangle(vec2 p1, vec2 p2, vec2 p3, vec2 c) 
	{
		float s1 = side(p1, p2, c);
		float s2 = side(p2, p3, c);
		float s3 = side(p3, p1, c);
		bool hasNeg, hasPos;
		hasNeg = (s1 < 0) || (s2 < 0) || (s3 < 0);
		hasPos = (s1 > 0) || (s2 > 0) || (s3 > 0);
		return !(hasNeg && hasPos);
	}

	bool inside_star(vec3 coord) 
	{
		vec2 c(coord.x, coord.y);
		vec2 p1(0, 1);
		vec2 p2(0.9510565163f, 0.30901699437f);
		vec2 p3(0.58778525229f, -0.80901699437f);
		vec2 p4(-0.58778525229f, -0.80901699437f);
		vec2 p5(-0.9510565163f, 0.30901699437f);

		vec2 o1 = (p2 + p3 + p4 + p5)/4.0f;
		vec2 o2 = (p1 + p3 + p4 + p5)/4.0f;
		vec2 o3 = (p1 + p2 + p4 + p5)/4.0f;
		vec2 o4 = (p1 + p2 + p3 + p5)/4.0f;
		vec2 o5 = (p1 + p2 + p3 + p4)/4.0f;

		return inside_triangle(p1, p3, o2, c) || inside_triangle(p2, p4, o3, c) 
			|| inside_triangle(p3, p5, o4, c) || inside_triangle(p4, p1, o5, c)
			|| inside_triangle(p5, p2, o1, c);
	}
	
	vec4 sample_color_map_opaque(vec3 coord)
	{
		float l = glm::length(coord);
		if (l < 0.25f) return vec4(0.2392f, 0.0235f, 0.0862f, 1.0f);
		if (l < 0.5f) return vec4(0.96f, 0.93f, 0.88f, 1.0f);
		if (l < 0.75f) return vec4(0.3529f, 0.7686f, 0.4078f, 1.0f);
		return vec4(0.96f, 0.93f, 0.88f, 1.0f);
	}

	vec4 sample_color_map_transparent(vec3 coord)
	{
		float l = glm::length(coord);
		if (l < 0.25f) return vec4(0.2392f, 0.0235f, 0.0862f, 1.0f);
		if (l < 0.5f) return vec4(0.96f, 0.93f, 0.88f, 0.02f);
		if (l < 0.75f) return vec4(0.3529f, 0.7686f, 0.4078f, 0.04f);
		return vec4(0.96f, 0.93f, 0.88f, 0.01f);
	}
	
	PointCloud* parseFromStream(istream& is)
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

	PointCloud* readPCD(istream& is)
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
		Point* points = new Point[pn];
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
							points[i] = p;
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
				points[i] = p;
			}
		} else {
			throw "Bad file format";
		}
		return new PointCloud(points, pn);
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

PointCloud::PointCloud(Point* points, size_t vn) 
{
	this->points = points;
	this->vn = vn;
	calculateBounds();
	createTree();
	calculatePointSize(PointSize::NearestAverage);
}

void PointCloud::SetPointSize(PointSize ps)
{
	calculatePointSize(ps);
}

void PointCloud::calculateBounds() 
{
	vec3 c = vec3(0, 0, 0);
	for (size_t i = 0; i < vn; i++) {
		c += points[i].position;
	}
	center = c / (float)vn;
	
	float r = 0;
	for (size_t i = 0; i < vn; i++) {
		float tr = glm::distance(center, points[i].position);
		if (r < tr) r = tr;
	}
	radius = r;
}

void PointCloud::createTree() 
{
	tree.model = points;
	tree.size = vn;
	tree.Construct();
}

void PointCloud::calculatePointSize(PointSize ps) 
{
	Point p;
	float sum = 0;
	for (size_t i = 0; i < vn; i++) {
		tree.NearestSearch(i, p);
		sum += glm::distance(p.position, points[i].position);
	}
	averagePointSize = sum / vn;
	switch (ps) {
	case PointSize::NearestMax:
		setNearestMax();
		return;
	case PointSize::NearestAverage:
		setNearestAverage();
		return;
	case PointSize::NearestAdaptative:
		setNearestAdaptative();
		return;
	case PointSize::NearestAverageAdaptative:
		setNearestAverageAdaptative();
		return;
	}
}

void PointCloud::setNearestMax() 
{
	Point p;
	float max = 0;
	float near;
	for (size_t i = 0; i < vn; i++) {
		tree.NearestSearch(i, p);
		near = glm::distance(p.position, points[i].position);
		if (max < near)max = near;
	}
	for (size_t i = 0; i < vn; i++) {
		points[i].pointSize = max;
	}
}

void PointCloud::setNearestAverage() 
{
	for (size_t i = 0; i < vn; i++) {
		points[i].pointSize = averagePointSize;
	}
}

void PointCloud::setNearestAdaptative() 
{
	Point p;
	for (size_t i = 0; i < vn; i++) {
		tree.NearestSearch(i, p);
		points[i].pointSize = glm::distance(p.position, points[i].position);
	}
}

void PointCloud::setNearestAverageAdaptative() 
{
	vector<Point> v;
	float space = averagePointSize * 2;
	float max = 0;
	for (size_t i = 0; i < vn; i++) {
		tree.NearestKSearch(i, 4, v);
		for (int j = 0; j < 4; j++) {
			float d = glm::distance(v[j].position, points[i].position);
			if (d < space && max < d)max = d;
		}
		points[i].pointSize = max;
		v.clear();
	}
}

PointCloud* PCReader::parseString(const char* str)
{
	istringstream iss(str);
	PointCloud* result = parseFromStream(iss);
	return result;
}

PointCloud* PCReader::parseFile(const char* fn)
{
	string p = string(fn);
	std::filesystem::path cwd = std::filesystem::current_path() / p;
	ifstream ifs;
	PointCloud* result;
	try{
		ifs.open(fn);
		if (ifs.good()) {
			result = parseFromStream(ifs);
		} else {
			cout << "ERROR: Point cloud file does not exist" << endl;
		}
		ifs.close();
	} catch (...) {
		cout << "ERROR: Point cloud file could not be read" << endl;
	}
	return result;
}

PointCloud* PCPrimitives::sample_star(size_t sampleRes)
{
	assert(sampleRes > 0);
	if (sampleRes > MAX_SAMPLE_RES)sampleRes = MAX_SAMPLE_RES;
	vector<Point> buffer;
	buffer.reserve(sampleRes * sampleRes);

	for (size_t i = 0; i < sampleRes; i++) {
		for (size_t j = 0; j < sampleRes; j++) {
			size_t ind = (i * sampleRes + j);
			float xoff = -1.0f + (float)i / (float)(sampleRes - 1) * 2.0f;
			float yoff = -1.0f + (float)j / (float)(sampleRes - 1) * 2.0f;
			vec3 pos = vec3(xoff, yoff, 0);
			vec4 color = sample_color_map_opaque(pos);
			Point p = Point(pos, color);
			if (inside_star(pos))buffer.push_back(p);
		}
	}
	size_t vn = buffer.size();
	Point* points = new Point[vn];
	std::copy(buffer.begin(), buffer.end(), points);
	return new PointCloud(points, vn);
}

PointCloud* PCPrimitives::sample_star_noisy(size_t sampleRes)
{
	assert(sampleRes > 0);
	if (sampleRes > MAX_SAMPLE_RES)sampleRes = MAX_SAMPLE_RES;
	vector<Point> buffer;
	buffer.reserve(sampleRes * sampleRes);

	for (size_t i = 0; i < sampleRes; i++) {
		for (size_t j = 0; j < sampleRes; j++) {
			size_t ind = (i * sampleRes + j);
			float x = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
			float y = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
			vec3 pos = vec3(x, y, 0);
			vec4 color = sample_color_map_opaque(pos);
			Point p = Point(pos, color);
			if (inside_star(pos))buffer.push_back(p);
		}
	}
	size_t vn = buffer.size();
	Point* points = new Point[vn];
	std::copy(buffer.begin(), buffer.end(), points);
	return new PointCloud(points, vn);
}

PointCloud* PCPrimitives::sample_slice(size_t sampleRes)
{
	assert(sampleRes > 0);
	if (sampleRes > MAX_SAMPLE_RES)sampleRes = MAX_SAMPLE_RES;
	size_t vn = sampleRes * sampleRes;
	Point* points = new Point[vn];
	
	for (size_t i = 0; i < sampleRes; i++) {
		for (size_t j = 0; j < sampleRes; j++) {
			size_t ind = (i * sampleRes + j);
			float xoff = -1.0f + (float)i / (float)(sampleRes - 1) * 2.0f;
			float yoff = -1.0f + (float)j / (float)(sampleRes - 1) * 2.0f;
			vec3 pos = vec3(xoff, yoff, 0);
			vec4 color = sample_color_map_opaque(pos);
			points[ind] = Point(pos, color);
		}
	}
	return new PointCloud(points, vn);
}

PointCloud* PCPrimitives::sample_slice_noisy(size_t sampleRes)
{
	assert(sampleRes > 0);
	if (sampleRes > MAX_SAMPLE_RES)sampleRes = MAX_SAMPLE_RES;
	size_t vn = sampleRes * sampleRes;
	Point* points = new Point[vn];

	for (size_t i = 0; i < sampleRes; i++) {
		for (size_t j = 0; j < sampleRes; j++) {
			size_t ind = (i * sampleRes + j);
			float x = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
			float y = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
			vec3 pos = vec3(x, y, 0);
			vec4 color = sample_color_map_opaque(pos);
			points[ind] = Point(pos, color);
		}
	}
	return new PointCloud(points, vn);
}

PointCloud* PCPrimitives::sample_cube_opaque(size_t sampleRes)
{
	assert(sampleRes > 0);
	if (sampleRes > MAX_SAMPLE_RES)sampleRes = MAX_SAMPLE_RES;
	size_t vn = sampleRes * sampleRes * sampleRes;
	Point* points = new Point[vn];

	for (size_t i = 0; i < sampleRes; i++) {
		for (size_t j = 0; j < sampleRes; j++) {
			for (size_t k = 0; k < sampleRes; k++) {
				size_t ind = i * sampleRes * sampleRes + j * sampleRes + k;
				float xoff = -1.0f + (float)i / (float)(sampleRes - 1) * 2.0f;
				float yoff = -1.0f + (float)j / (float)(sampleRes - 1) * 2.0f;
				float zoff = -1.0f + (float)k / (float)(sampleRes - 1) * 2.0f;
				vec3 pos = vec3(xoff, yoff, zoff);
				vec4 color = sample_color_map_opaque(pos);
				points[ind] = Point(pos, color);
			}
		}
	}
	return new PointCloud(points, vn);
}

PointCloud* PCPrimitives::sample_cube_opaque_noisy(size_t sampleRes)
{
	assert(sampleRes > 0);
	if (sampleRes > MAX_SAMPLE_RES)sampleRes = MAX_SAMPLE_RES;
	size_t vn = sampleRes * sampleRes * sampleRes;
	Point* points = new Point[vn];

	for (size_t i = 0; i < sampleRes; i++) {
		for (size_t j = 0; j < sampleRes; j++) {
			for (size_t k = 0; k < sampleRes; k++) {
				size_t ind = i * sampleRes * sampleRes + j * sampleRes + k;
				float x = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
				float y = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
				float z = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
				vec3 pos = vec3(x, y, z);
				vec4 color = sample_color_map_opaque(pos);
				points[ind] = Point(pos, color);
			}
		}
	}
	return new PointCloud(points, vn);
}

PointCloud* PCPrimitives::sample_cube_transparent(size_t sampleRes)
{
	assert(sampleRes > 0);
	if (sampleRes > MAX_SAMPLE_RES)sampleRes = MAX_SAMPLE_RES;
	size_t vn = sampleRes * sampleRes * sampleRes;
	Point* points = new Point[vn];

	for (size_t i = 0; i < sampleRes; i++) {
		for (size_t j = 0; j < sampleRes; j++) {
			for (size_t k = 0; k < sampleRes; k++) {
				size_t ind = i * sampleRes * sampleRes + j * sampleRes + k;
				float xoff = -1.0f + (float)i / (float)(sampleRes - 1) * 2.0f;
				float yoff = -1.0f + (float)j / (float)(sampleRes - 1) * 2.0f;
				float zoff = -1.0f + (float)k / (float)(sampleRes - 1) * 2.0f;
				vec3 pos = vec3(xoff, yoff, zoff);
				vec4 color = sample_color_map_transparent(pos);
				points[ind] = Point(pos, color);
			}
		}
	}
	return new PointCloud(points, vn);
}

PointCloud* PCPrimitives::sample_cube_transparent_noisy(size_t sampleRes)
{
	assert(sampleRes > 0);
	if (sampleRes > MAX_SAMPLE_RES)sampleRes = MAX_SAMPLE_RES;
	size_t vn = sampleRes * sampleRes * sampleRes;
	Point* points = new Point[vn];

	for (size_t i = 0; i < sampleRes; i++) {
		for (size_t j = 0; j < sampleRes; j++) {
			for (size_t k = 0; k < sampleRes; k++) {
				size_t ind = i * sampleRes * sampleRes + j * sampleRes + k;
				float x = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
				float y = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
				float z = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
				vec3 pos = vec3(x, y, z);
				vec4 color = sample_color_map_transparent(pos);
				points[ind] = Point(pos, color);
			}
		}
	}
	return new PointCloud(points, vn);
}



