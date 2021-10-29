#pragma once

#include "point.h"
#include <glm/glm.hpp>
#include <algorithm>
#include <vector>
#include "search_queue.h"
#include <queue>




class KdTree {
private:
	struct Bounds {
		enum class Axis {
			XAxis, YAxis, ZAxis
		};
		enum class Partition {
			Left, Right
		};
		static constexpr float neg_inf = -std::numeric_limits<float>().max();
		static constexpr float pos_inf = std::numeric_limits<float>().max();
		float minX, minY, minZ;
		float maxX, maxY, maxZ;
		Bounds();
		Bounds(Axis ax, Partition part, float offset);
		Bounds(glm::vec3 p);
		void Include(glm::vec3 p);
		void Unite(Bounds b);
		void Intersect(Bounds b);
		float InsideDistance(glm::vec3 p) const;
		float OutsideDistance(glm::vec3 p) const;
		float AxisMinValue(glm::vec3 axis) const;
		float AxisMaxValue(glm::vec3 axis) const;
		bool ContainsAxisRange(glm::vec3 axis, float from, float to) const;
		static float Distance(glm::vec3 p1, glm::vec3 p2);
		static bool InsideAxisRange(glm::vec3 point, glm::vec3 axis, float from, float to);
		static float AxisValue(glm::vec3 axis, glm::vec3 point);
	};
	
	struct KdNode {
		uint32_t pointInd;
		KdNode* parent;
		KdNode* left;
		KdNode* right;
		Bounds minBounds;
		Bounds maxBounds;
		KdNode() : pointInd(0), parent(nullptr), left(nullptr), right(nullptr) {}
	};

	struct NodeSearch {
		KdNode* node;
		KdNode* previous;
		float searchValue;
		NodeSearch() : node(nullptr), previous(nullptr), searchValue(0)
		{}
		NodeSearch(KdNode* n, KdNode* prev, float d) : node(n), previous(prev), searchValue(d)
		{}
	};

	struct KNResult {
		float dist;
		uint32_t pointInd;
		KNResult() : pointInd(0), dist(0)
		{}
		KNResult(uint32_t pind, float d) : pointInd(pind), dist(d)
		{}
		inline bool operator >(const KNResult& r) const { return this->dist > r.dist; }
	};

	struct MinSearch {
		bool operator() (NodeSearch n1, NodeSearch n2) { return n1.searchValue < n2.searchValue; }
	};

	struct MaxSearch {
		bool operator() (NodeSearch n1, NodeSearch n2) { return n1.searchValue > n2.searchValue; }
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

	NormalQueue<NodeSearch> searchQueue;
	PriorityQueue<NodeSearch, MinSearch> minSearchQueue;
	PriorityQueue<NodeSearch, MaxSearch> maxSearchQueue;
	PriorityQueue<KNResult> resultsQueue;

	void minAxis(glm::vec3 axis, uint32_t& resultPoint, float& resultValue);
	void maxAxis(glm::vec3 axis, uint32_t& resultPoint, float& resultValue);
	void rangeAxis(glm::vec3 axis, float from, float to, std::vector<uint32_t>& resultPoints);

	static KdNode* _subnode_axis(KdNode* graphArray, Point* points, KdNode* parent, uint32_t* p_axis, uint32_t* s_axis1, uint32_t* s_axis2,
		uint32_t* p_axis_ind, uint32_t* s_axis1_ind, uint32_t* s_axis2_ind,
		uint32_t* buff1, uint32_t* buff2,
		uint32_t from, uint32_t to, 
		Bounds maxBounds, Bounds::Axis axis);

public:
	Point* model;
	uint32_t size;
	uint32_t _expandedNodes;
	KdTree();
	void Construct();
	void NearestSearch(uint32_t pointIndex, uint32_t& result);
	void NearestSearch(glm::vec3 pos, uint32_t& result);
	void NearestKSearch(uint32_t pointIndex, unsigned int k, std::vector<uint32_t>& result);
	void NearestKSearch(glm::vec3 pos, unsigned int k, std::vector<uint32_t>& result);

	void AxisMin(glm::vec3 axis, uint32_t& resultPoint, float& resultValue);
	void AxisMax(glm::vec3 axis, uint32_t& resultPoint, float& resultValue);
	void AxisRange(glm::vec3 axis, float from, float to, std::vector<uint32_t>& resultPoints);
};
