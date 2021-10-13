#pragma once

#include "point.h"
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
		float MinAxisValue(glm::vec3 axis) const;
		float MaxAxisValue(glm::vec3 axis) const;
		bool ContainsAxisRange(glm::vec3 axis, float from, float to) const;
		static float Distance(glm::vec3 p1, glm::vec3 p2);
		static bool InsideAxisRange(glm::vec3 point, glm::vec3 axis, float from, float to);
	};
	
	struct KdNode {
		uint32_t pointInd;
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
		bool operator() (uint32_t i, uint32_t j) { return model[i].position.x < model[j].position.x; }
	}compareX;

	struct {
		Point* model;
		bool operator() (uint32_t i, uint32_t j) { return model[i].position.y < model[j].position.y; }
	}compareY;

	struct {
		Point* model;
		bool operator() (uint32_t i, uint32_t j) { return model[i].position.z < model[j].position.z; }
	}compareZ;

	KdNode* treeArray;
	KdNode* root;
	uint32_t* x_axis;
	uint32_t* y_axis;
	uint32_t* z_axis;
	uint32_t* x_axis_ind;
	uint32_t* y_axis_ind;
	uint32_t* z_axis_ind;
	uint32_t _expandedNodes;

	Point nearestSearch(const KdNode* start, glm::vec3 position, bool includeStart);
	Point nearestSearch2(const KdNode* start, glm::vec3 position, bool includeStart);
	void minAxis(glm::vec3 axis, uint32_t& resultPoint, float& resultValue);
	void maxAxis(glm::vec3 axis, uint32_t& resultPoint, float& resultValue);
	void rangeAxis(glm::vec3 axis, float from, float to, std::vector<uint32_t>& resultPoints);
	void nearestKSearch(const KdNode* start, glm::vec3 position, unsigned int k, bool includeStart, std::vector<Point>& result);

	static KdNode* _subnode_axis(KdNode* graphArray, Point* points, KdNode* parent, uint32_t* p_axis, uint32_t* s_axis1, uint32_t* s_axis2,
		uint32_t* p_axis_ind, uint32_t* s_axis1_ind, uint32_t* s_axis2_ind,
		uint32_t* buff1, uint32_t* buff2,
		uint32_t from, uint32_t to);

public:
	Point* model;
	uint32_t size;
	KdTree();
	void Construct();
	void NearestSearch(uint32_t pointIndex, Point& result);
	void NearestSearchBenchmark1(uint32_t pointIndex, Point& result, uint32_t& expandedNodes);
	void NearestSearchBenchmark2(uint32_t pointIndex, Point& result, uint32_t& expandedNodes);
	void NearestSearch(glm::vec3 pos, Point& result);
	void NearestKSearch(uint32_t pointIndex, unsigned int k, std::vector<Point>& result);
	void NearestKSearch(glm::vec3 pos, unsigned int k, std::vector<Point>& result);

	void MinAxis(glm::vec3 axis, uint32_t& resultPoint, float& resultValue);
	void MaxAxis(glm::vec3 axis, uint32_t& resultPoint, float& resultValue);
	void RangeAxis(glm::vec3 axis, float from, float to, std::vector<uint32_t>& resultPoints);
};
