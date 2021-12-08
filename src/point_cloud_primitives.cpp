#include "point_cloud_primitives.h"
#include <glm/glm.hpp>
#include <vector>

using namespace glm;

namespace {
	
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

		vec2 o1 = (p2 + p3 + p4 + p5) / 4.0f;
		vec2 o2 = (p1 + p3 + p4 + p5) / 4.0f;
		vec2 o3 = (p1 + p2 + p4 + p5) / 4.0f;
		vec2 o4 = (p1 + p2 + p3 + p5) / 4.0f;
		vec2 o5 = (p1 + p2 + p3 + p4) / 4.0f;

		return inside_triangle(p1, p3, o2, c) || inside_triangle(p2, p4, o3, c)
			|| inside_triangle(p3, p5, o4, c) || inside_triangle(p4, p1, o5, c)
			|| inside_triangle(p5, p2, o1, c);
	}

	ivec3 menger_coord(vec3 center, float size, vec3 pos)
	{
		pos = pos - center;
		int x = 0, y = 0, z = 0;
		if (pos.x > size)x = 1;
		else if (pos.x < -size)x = -1;
		if (pos.y > size) y = 1;
		else if (pos.y < size)y = -1;
		if (pos.z > size)z = 1;
		else if (pos.z < -size)z = -1;
		return ivec3(x, y, z);
	}

	int menger_cube_layer(vec3 pos, uint32_t iterations, float offset) 
	{
		float size = 1.0f;
		vec3 center = vec3(0, 0, 0);
		int layer = iterations+1;
		for (int n = 0; n < iterations; n++) {
			ivec3 coord = menger_coord(center, size, pos);
			if ((coord.x == 0 && coord.y == 0) || 
				(coord.x == 0 && coord.z == 0) || 
				(coord.y == 0 && coord.z == 0)) {
				return 0;
			}
			
			vec3 v = pos - center;
			float norm = max(abs(v.x), max(abs(v.y), abs(v.z)));
			if (norm >= size-offset-std::numeric_limits<float>().min() && layer == iterations + 1) {
				layer = n + 1;
			}
			center = (vec3)coord * 2.0f * size;
			size /= 3.0f;
		}
		return layer;
	}

	vec4 color_map_opaque_1(vec3 coord)
	{
		float l = glm::length(coord);
		if (l < 0.25f) return vec4(0.2392f, 0.0235f, 0.0862f, 1.0f);
		if (l < 0.5f) return vec4(0.96f, 0.93f, 0.88f, 1.0f);
		if (l < 0.75f) return vec4(0.3529f, 0.7686f, 0.4078f, 1.0f);
		return vec4(0.96f, 0.93f, 0.88f, 1.0f);
	}

	vec4 color_map_transparent_1(vec3 coord)
	{
		float l = glm::length(coord);
		if (l < 0.25f) return vec4(0.2392f, 0.0235f, 0.0862f, 1.0f);
		if (l < 0.5f) return vec4(0.96f, 0.93f, 0.88f, 0.02f);
		if (l < 0.75f) return vec4(0.3529f, 0.7686f, 0.4078f, 0.04f);
		return vec4(0.96f, 0.93f, 0.88f, 0.02f);
	}

	vec4 color_map_transparent_2(vec3 coord)
	{
		float l = glm::length(coord);
		if (l < 0.25f) return vec4(0.2392f, 0.0235f, 0.0862f, 1.0f);
		if (l < 0.5f) return vec4(1.0f, 1.0f, 1.0f, 0.2f);
		if (l < 0.75f) return vec4(0.3529f, 0.7686f, 0.4078f, 0.4f);
		return vec4(1.0f, 1.0f, 1.0f, 0.1f);
	}
}

PointCloud* PCPrimitives::star(uint32_t sampleRes)
{
	if (sampleRes > MAX_SQUARE_SAMPLE_RES)sampleRes = MAX_SQUARE_SAMPLE_RES;
	uint32_t vn = sampleRes * sampleRes;
	std::vector<Point> buffer;
	buffer.reserve(vn);

	for (size_t i = 0; i < sampleRes; i++) {
		for (size_t j = 0; j < sampleRes; j++) {
			size_t ind = (i * sampleRes + j);
			float xoff = -1.0f + (float)i / (float)(sampleRes - 1) * 2.0f;
			float yoff = -1.0f + (float)j / (float)(sampleRes - 1) * 2.0f;
			vec3 pos = vec3(xoff, yoff, 0);
			vec4 color = color_map_opaque_1(pos);
			Point p = Point(pos, color, vec3(0,0,1));
			if (inside_star(pos))buffer.push_back(p);
		}
	}
	return new PointCloud(buffer);
}

PointCloud* PCPrimitives::star_noisy(uint32_t sampleRes)
{
	if (sampleRes > MAX_SQUARE_SAMPLE_RES)sampleRes = MAX_SQUARE_SAMPLE_RES;
	uint32_t vn = sampleRes * sampleRes;
	std::vector<Point> buffer;
	buffer.reserve(vn);

	for (size_t i = 0; i < sampleRes; i++) {
		for (size_t j = 0; j < sampleRes; j++) {
			size_t ind = (i * sampleRes + j);
			float x = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
			float y = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
			vec3 pos = vec3(x, y, 0);
			vec4 color = color_map_opaque_1(pos);
			Point p = Point(pos, color, vec3(0, 0, 1));
			if (inside_star(pos))buffer.push_back(p);
		}
	}
	return new PointCloud(buffer);
}

PointCloud* PCPrimitives::slice(uint32_t sampleRes)
{
	if (sampleRes > MAX_SQUARE_SAMPLE_RES)sampleRes = MAX_SQUARE_SAMPLE_RES;
	uint32_t vn = sampleRes * sampleRes;
	std::vector<Point>points(vn);

	for (size_t i = 0; i < sampleRes; i++) {
		for (size_t j = 0; j < sampleRes; j++) {
			size_t ind = (i * sampleRes + j);
			float xoff = -1.0f + (float)i / (float)(sampleRes - 1) * 2.0f;
			float yoff = -1.0f + (float)j / (float)(sampleRes - 1) * 2.0f;
			vec3 pos = vec3(xoff, yoff, 0);
			vec4 color = color_map_opaque_1(pos);
			points[ind] = Point(pos, color, vec3(0, 0, 1));
		}
	}
	return new PointCloud(points);
}

PointCloud* PCPrimitives::slice_noisy(uint32_t sampleRes)
{
	if (sampleRes > MAX_SQUARE_SAMPLE_RES)sampleRes = MAX_SQUARE_SAMPLE_RES;
	uint32_t vn = sampleRes * sampleRes;
	std::vector<Point>points(vn);

	for (size_t i = 0; i < sampleRes; i++) {
		for (size_t j = 0; j < sampleRes; j++) {
			size_t ind = (i * sampleRes + j);
			float x = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
			float y = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
			vec3 pos = vec3(x, y, 0);
			vec4 color = color_map_opaque_1(pos);
			points[ind] = Point(pos, color, vec3(0, 0, 1));
		}
	}
	return new PointCloud(points);
}

PointCloud* PCPrimitives::slice_transparent(uint32_t sampleRes)
{
	if (sampleRes > MAX_SQUARE_SAMPLE_RES)sampleRes = MAX_SQUARE_SAMPLE_RES;
	uint32_t vn = sampleRes * sampleRes;
	std::vector<Point>points(vn);

	for (size_t i = 0; i < sampleRes; i++) {
		for (size_t j = 0; j < sampleRes; j++) {
			size_t ind = (i * sampleRes + j);
			float xoff = -1.0f + (float)i / (float)(sampleRes - 1) * 2.0f;
			float yoff = -1.0f + (float)j / (float)(sampleRes - 1) * 2.0f;
			vec3 pos = vec3(xoff, yoff, 0);
			vec4 color = color_map_transparent_2(pos);
			points[ind] = Point(pos, color, vec3(0, 0, 1));
		}
	}
	return new PointCloud(points);
}

PointCloud* PCPrimitives::cube_opaque(uint32_t sampleRes)
{
	if (sampleRes > MAX_CUBE_SAMPLE_RES)sampleRes = MAX_CUBE_SAMPLE_RES;
	uint32_t vn = sampleRes * sampleRes * sampleRes;
	std::vector<Point>points(vn);

	for (size_t i = 0; i < sampleRes; i++) {
		for (size_t j = 0; j < sampleRes; j++) {
			for (size_t k = 0; k < sampleRes; k++) {
				size_t ind = i * sampleRes * sampleRes + j * sampleRes + k;
				float xoff = -1.0f + (float)i / (float)(sampleRes - 1) * 2.0f;
				float yoff = -1.0f + (float)j / (float)(sampleRes - 1) * 2.0f;
				float zoff = -1.0f + (float)k / (float)(sampleRes - 1) * 2.0f;
				vec3 pos = vec3(xoff, yoff, zoff);
				vec4 color = color_map_opaque_1(pos);
				vec3 norm(0, 0, 0);
				float wx = abs(xoff);
				float wy = abs(yoff);
				float wz = abs(zoff);
				if (wx > wy && wx > wz) {
					norm = xoff > 0 ? vec3(1, 0, 0) : vec3(-1, 0, 0);
				}
				else if (wy > wx && wy > wz) {
					norm = yoff > 0 ? vec3(0, 1, 0) : vec3(0, -1, 0);
				}
				else if (wz > wx && wz > wy) {
					norm = zoff > 0 ? vec3(0, 0, 1) : vec3(0, 0, -1);
				}
				else if (wx == wy) {
					norm = glm::normalize(vec3(xoff, yoff, 0));
				}
				else if (wx == wz) {
					norm = glm::normalize(vec3(xoff, 0, zoff));
				}
				else if (wy == wz) {
					norm = glm::normalize(vec3(0, yoff, zoff));
				}
				else {
					norm = glm::normalize(pos);
				}
				points[ind] = Point(pos, color, norm);
			}
		}
	}
	return new PointCloud(points);
}

PointCloud* PCPrimitives::cube_transparent(uint32_t sampleRes)
{
	if (sampleRes > MAX_CUBE_SAMPLE_RES)sampleRes = MAX_CUBE_SAMPLE_RES;
	uint32_t vn = sampleRes * sampleRes * sampleRes;
	std::vector<Point>points(vn);

	for (size_t i = 0; i < sampleRes; i++) {
		for (size_t j = 0; j < sampleRes; j++) {
			for (size_t k = 0; k < sampleRes; k++) {
				size_t ind = i * sampleRes * sampleRes + j * sampleRes + k;
				float xoff = -1.0f + (float)i / (float)(sampleRes - 1) * 2.0f;
				float yoff = -1.0f + (float)j / (float)(sampleRes - 1) * 2.0f;
				float zoff = -1.0f + (float)k / (float)(sampleRes - 1) * 2.0f;
				vec3 pos = vec3(xoff, yoff, zoff);
				vec4 color = color_map_transparent_1(pos);
				vec3 norm(0, 0, 0);
				float wx = abs(xoff);
				float wy = abs(yoff);
				float wz = abs(zoff);
				if (wx > wy && wx > wz) {
					norm = xoff > 0 ? vec3(1, 0, 0) : vec3(-1,0,0);
				}
				else if (wy > wx && wy > wz) {
					norm = yoff > 0 ? vec3(0, 1, 0) : vec3(0, -1, 0);
				}
				else if (wz > wx && wz > wy) {
					norm = zoff > 0 ? vec3(0, 0, 1) : vec3(0, 0, -1);
				}
				else if (wx == wy) {
					norm = glm::normalize(vec3(xoff, yoff, 0));
				}
				else if (wx == wz) {
					norm = glm::normalize(vec3(xoff, 0, zoff));
				}
				else if (wy == wz) {
					norm = glm::normalize(vec3(0, yoff, zoff));
				}
				else {
					norm = glm::normalize(pos);
				}

				points[ind] = Point(pos, color, norm);
			}
		}
	}
	return new PointCloud(points);
}

PointCloud* PCPrimitives::sphere_opaque(uint32_t sampleRes)
{
	if (sampleRes > MAX_CUBE_SAMPLE_RES)sampleRes = MAX_CUBE_SAMPLE_RES;
	std::vector<Point> buffer;
	
	for (size_t i = 0; i < sampleRes; i++) {
		for (size_t j = 0; j < sampleRes; j++) {
			for (size_t k = 0; k < sampleRes; k++) {
				size_t ind = i * sampleRes * sampleRes + j * sampleRes + k;
				float xoff = -1.0f + (float)i / (float)(sampleRes - 1) * 2.0f;
				float yoff = -1.0f + (float)j / (float)(sampleRes - 1) * 2.0f;
				float zoff = -1.0f + (float)k / (float)(sampleRes - 1) * 2.0f;
				vec3 pos = vec3(xoff, yoff, zoff);
				vec4 color = color_map_opaque_1(pos);
				vec3 norm = pos != vec3(0, 0, 0) ? glm::normalize(pos) : vec3(0, 0, 0);
				if (glm::length(pos) <= 1) {
					buffer.push_back(Point(pos, color, norm));
				}
			}
		}
	}
	return new PointCloud(buffer);
}

PointCloud* PCPrimitives::sphere_transparent(uint32_t sampleRes)
{
	if (sampleRes > MAX_CUBE_SAMPLE_RES)sampleRes = MAX_CUBE_SAMPLE_RES;
	std::vector<Point> buffer;

	for (size_t i = 0; i < sampleRes; i++) {
		for (size_t j = 0; j < sampleRes; j++) {
			for (size_t k = 0; k < sampleRes; k++) {
				size_t ind = i * sampleRes * sampleRes + j * sampleRes + k;
				float xoff = -1.0f + (float)i / (float)(sampleRes - 1) * 2.0f;
				float yoff = -1.0f + (float)j / (float)(sampleRes - 1) * 2.0f;
				float zoff = -1.0f + (float)k / (float)(sampleRes - 1) * 2.0f;
				vec3 pos = vec3(xoff, yoff, zoff);
				vec4 color = color_map_transparent_1(pos);
				vec3 norm = pos != vec3(0, 0, 0) ? glm::normalize(pos) : vec3(0, 0, 0);
				if (glm::length(pos) <= 1) {
					buffer.push_back(Point(pos, color, norm));
				}
			}
		}
	}
	return new PointCloud(buffer);
}

PointCloud* PCPrimitives::menger_cube(uint32_t sampleRes)
{
	return nullptr;
}

PointCloud* PCPrimitives::menger_cube_transparent(uint32_t sampleRes)
{
	return nullptr;
}

