#pragma once

#include "point_cloud.h"
#include <glm/glm.hpp>
#include <algorithm>
#include <vector>



class KdTree {
private:
	struct Bounds {
		float minX, minY, minZ;
		float maxX, maxY, maxZ;
		Bounds() : minX(0), minY(0), minZ(0), maxX(0), maxY(0), maxZ(0)
		{}
		Bounds(glm::vec3 p) : minX(p.x), maxX(p.x), minY(p.y), maxY(p.y), minZ(p.z), maxZ(p.z)
		{}
		void Include(glm::vec3 p);
		void Unite(Bounds p2);
		bool Contains(glm::vec3 p) const;
		float Distance(glm::vec3 p) const;
		static float Distance(glm::vec3 p1, glm::vec3 p2);
	};
	
	struct KdNode {
		size_t pointInd;
		KdNode* parent;
		KdNode* left;
		KdNode* right;
		Bounds bounds;
		Bounds leftBounds;
		Bounds rightBounds;
		KdNode() : pointInd(0), parent(nullptr), left(nullptr), right(nullptr) {}
	};

	struct NodeQuery {
		const KdNode* node;
		const KdNode* previous;
		float lowBound;
		NodeQuery() : node(nullptr), previous(nullptr), lowBound(-1)
		{}
		NodeQuery(const KdNode* n, const KdNode* prev, float d) : node(n), previous(prev), lowBound(d)
		{}
		bool operator <(const NodeQuery& r) const { return this->lowBound > r.lowBound; }
	};

	struct{
		Point* model;
		bool operator() (size_t i, size_t j) { return model[i].position.x < model[j].position.x; }
	}compareX;

	struct {
		Point* model;
		bool operator() (size_t i, size_t j) { return model[i].position.y < model[j].position.y; }
	}compareY;

	struct {
		Point* model;
		bool operator() (size_t i, size_t j) { return model[i].position.z < model[j].position.z; }
	}compareZ;

	KdNode* treeArray;
	KdNode* root;
	size_t* x_axis;
	size_t* y_axis;
	size_t* z_axis;
	size_t* x_axis_ind;
	size_t* y_axis_ind;
	size_t* z_axis_ind;
	size_t _expandedNodes;

	Point nearestSearch(const KdNode* start, glm::vec3 position, bool includeStart);
	Point nearestSearch2(const KdNode* start, glm::vec3 position, bool includeStart);
	std::vector<Point> nearestKSearch(const KdNode* start, glm::vec3 position, unsigned int k, bool includeStart);

	static KdNode* _subnode_axis(KdNode* graphArray, Point* points, KdNode* parent, size_t* p_axis, size_t* s_axis1, size_t* s_axis2,
		size_t* p_axis_ind, size_t* s_axis1_ind, size_t* s_axis2_ind,
		size_t* buff1, size_t* buff2,
		size_t from, size_t to);

public:
	Point* model;
	size_t size;
	KdTree();
	void Construct();
	void NearestSearch(size_t pointIndex, Point& result);
	void NearestSearchBenchmark1(size_t pointIndex, Point& result, size_t& expandedNodes);
	void NearestSearchBenchmark2(size_t pointIndex, Point& result, size_t& expandedNodes);
	void NearestSearch(glm::vec3 pos, Point& result);
	void NearestKSearch(size_t pointIndex, unsigned int k, std::vector<Point>& result);
	void NearestKSearch(glm::vec3 pos, unsigned int k, std::vector<Point>& result);
};
