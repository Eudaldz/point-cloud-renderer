#pragma once

#include "point_cloud.h"
#include <glm/glm.hpp>
#include <algorithm>
#include <vector>

struct Bounds {
	float minX, minY, minZ;
	float maxX, maxY, maxZ;
	Bounds(): minX(0), minY(0), minZ(0), maxX(0), maxY(0), maxZ(0) 
	{}
	Bounds(glm::vec3 p): minX(p.x), maxX(p.x), minY(p.y), maxY(p.y), minZ(p.z), maxZ(p.z) 
	{}
	
	void Include(glm::vec3 p)
	{
		minX = std::min(minX, p.x);
		minY = std::min(minY, p.y);
		minZ = std::min(minZ, p.z);
		maxX = std::max(minX, p.x);
		maxY = std::max(minY, p.y);
		maxZ = std::max(minZ, p.z);
	}

	void Unite(Bounds p2) 
	{
		minX = std::min(minX, p2.minX);
		minY = std::min(minY, p2.minY);
		minZ = std::min(minZ, p2.minZ);
		maxX = std::max(maxX, p2.maxX);
		maxY = std::max(maxY, p2.maxY);
		maxZ = std::max(maxZ, p2.maxZ);
	}

	bool Contains(glm::vec3 p) const
	{
		return minX <= p.x && p.x <= maxX &&
			minY <= p.y && p.y <= maxY &&
			minZ <= p.z && p.z <= maxZ;
	}

	float Distance(glm::vec3 p) const
	{
		float dx = 0;
		float dy = 0;
		float dz = 0;
		if (p.x < minX)dx = minX - p.x;
		else if (p.x > maxX)dx = p.x - maxX;
		if (p.y < minY)dy = minY - p.y;
		else if (p.y > maxY)dy = p.y - maxY;
		if (p.z < minZ)dz = minZ - p.z;
		else if (p.z > maxZ)dx = p.z - maxZ;
		return dx * dx + dy * dy + dz * dz;
	}

	static float Distance(glm::vec3 p1, glm::vec3 p2) {
		return glm::dot(p1, p2);
	}
};

struct KdNode{
	size_t pointInd;
	KdNode* parent;
	KdNode* left;
	KdNode* right;
	Bounds bounds;
	Bounds leftBounds;
	Bounds rightBounds;
	KdNode(): pointInd(0), parent(nullptr), left(nullptr), right(nullptr){}
};

struct PointInd {
	size_t x_ind;
	size_t y_ind;
	size_t z_ind;
};


class KdTree {
private:
	KdNode* treeArray;
	KdNode* root;
	size_t* x_axis;
	size_t* y_axis;
	size_t* z_axis;
	size_t* x_axis_ind;
	size_t* y_axis_ind;
	size_t* z_axis_ind;

	Point nearestSearch(const KdNode* start, glm::vec3 position, bool includeStart);
	std::vector<Point> nearestKSearch(const KdNode* start, glm::vec3 position, unsigned int k, bool includeStart);
	bool sort_x(size_t a, size_t b);
	bool sort_y(size_t a, size_t b);
	bool sort_z(size_t a, size_t b);

public:
	Point* model;
	size_t size;
	KdTree();
	void Construct();
	void NearestSearch(size_t pointIndex, Point& result);
	void NearestSearch(glm::vec3 pos, Point& result);
	void NearestKSearch(size_t pointIndex, unsigned int k, std::vector<Point>& result);
	void NearestKSearch(glm::vec3 pos, unsigned int k, std::vector<Point>& result);
};
