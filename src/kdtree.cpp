#include "kdtree.h"
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <queue>

using namespace std;
using namespace glm;

namespace {
	struct KResult 
	{
		float dist;
		uint32_t pointInd;

		KResult(): dist(0), pointInd(-1)
		{}

		KResult(uint32_t pind, float d): pointInd(pind), dist(d)
		{}

		inline bool operator <(const KResult& r) { return this->dist < r.dist; }
	};
}

void KdTree::Bounds::Include(glm::vec3 p)
{
	minX = std::min(minX, p.x);
	minY = std::min(minY, p.y);
	minZ = std::min(minZ, p.z);
	maxX = std::max(minX, p.x);
	maxY = std::max(minY, p.y);
	maxZ = std::max(minZ, p.z);
}

void KdTree::Bounds::Unite(Bounds p2)
{
	minX = std::min(minX, p2.minX);
	minY = std::min(minY, p2.minY);
	minZ = std::min(minZ, p2.minZ);
	maxX = std::max(maxX, p2.maxX);
	maxY = std::max(maxY, p2.maxY);
	maxZ = std::max(maxZ, p2.maxZ);
}

bool inline KdTree::Bounds::Contains(glm::vec3 p) const
{
	return minX <= p.x && p.x <= maxX &&
		minY <= p.y && p.y <= maxY &&
		minZ <= p.z && p.z <= maxZ;
}

float inline KdTree::Bounds::Distance(glm::vec3 p) const
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

float inline KdTree::Bounds::MinAxisValue(glm::vec3 v) const
{
	float dx = v.x >= 0 ? minX : maxX;
	float dy = v.y >= 0 ? minY : maxY;
	float dz = v.z >= 0 ? minZ : maxZ;
	return glm::dot(v, vec3(dx, dy, dz));
}

float inline KdTree::Bounds::MaxAxisValue(glm::vec3 v) const
{
	float dx = v.x >= 0 ? maxX : minX;
	float dy = v.y >= 0 ? maxY : minY;
	float dz = v.z >= 0 ? maxZ : minZ;
	return glm::dot(v, vec3(dx, dy, dz));
}

bool inline KdTree::Bounds::ContainsAxisRange(glm::vec3 axis, float from, float to) const
{
	float minv = MinAxisValue(axis);
	float maxv = MaxAxisValue(axis);
	return !(minv >= to || maxv < from);
}

float inline KdTree::Bounds::Distance(glm::vec3 p1, glm::vec3 p2) {
	vec3 l = p1 - p2;
	return glm::dot(l, l);
}

bool inline KdTree::Bounds::InsideAxisRange(glm::vec3 point, glm::vec3 axis, float from, float to) {
	float d = glm::dot(point, axis);
	return d >= from && d < to;
}

KdTree::KdNode* KdTree::_subnode_axis(KdTree::KdNode* graphArray, Point* points, KdTree::KdNode* parent, uint32_t* p_axis, uint32_t* s_axis1, uint32_t* s_axis2,
	uint32_t* p_axis_ind, uint32_t* s_axis1_ind, uint32_t* s_axis2_ind,
	uint32_t* buff1, uint32_t* buff2,
	uint32_t from, uint32_t to)
{
	uint32_t mid = from + (to - from) / 2;
	uint32_t medianInd = p_axis[mid];
	KdNode* current = &graphArray[medianInd];
	current->parent = parent;
	current->pointInd = medianInd;
	current->bounds = Bounds(points[medianInd].position);
	current->leftBounds = Bounds(points[medianInd].position);
	current->rightBounds = Bounds(points[medianInd].position);
	uint32_t l1 = from, r1 = mid + 1, l2 = from, r2 = mid + 1;

	buff1[mid] = medianInd;
	buff2[mid] = medianInd;
	s_axis1_ind[medianInd] = mid;
	s_axis2_ind[medianInd] = mid;

	for (uint32_t i = from; i < to; i++) {
		uint32_t c1 = s_axis1[i];
		if (p_axis_ind[c1] < mid) {
			buff1[l1] = c1;
			s_axis1_ind[c1] = l1;
			l1++;
		}
		else if (p_axis_ind[c1] > mid) {
			buff1[r1] = c1;
			s_axis1_ind[c1] = r1;
			r1++;
		}
		uint32_t c2 = s_axis2[i];
		if (p_axis_ind[c2] < mid) {
			buff2[l2] = c2;
			s_axis2_ind[c2] = l2;
			l2++;
		}
		else if (p_axis_ind[c2] > mid) {
			buff2[r2] = c2;
			s_axis2_ind[c2] = r2;
			r2++;
		}
	}
	std::copy(buff1 + from, buff1 + to, s_axis1 + from);
	std::copy(buff2 + from, buff2 + to, s_axis2 + from);

	//cout << "[ " << (parent != nullptr ? to_string(parent->pointInd) : "NULL") << "] -> " << current->pointInd << endl;
	current->left = nullptr;
	if (from < mid) {
		current->left = _subnode_axis(graphArray, points, current, s_axis1, s_axis2, p_axis,
			s_axis1_ind, s_axis2_ind, p_axis_ind, buff1, buff2, from, mid);
		current->bounds.Unite(current->left->bounds);
		current->leftBounds.Unite(current->left->bounds);
	}
	current->right = nullptr;
	if (mid + 1 < to) {
		current->right = _subnode_axis(graphArray, points, current, s_axis1, s_axis2, p_axis,
			s_axis1_ind, s_axis2_ind, p_axis_ind, buff1, buff2, mid + 1, to);
		current->bounds.Unite(current->right->bounds);
		current->rightBounds.Unite(current->right->bounds);
	}
	
	return current;
}


KdTree::KdTree() 
{
	size = 0;
	treeArray = nullptr;
	model = nullptr;
	x_axis = nullptr;
	y_axis = nullptr;
	z_axis = nullptr;
}

void KdTree::Construct() 
{
	treeArray = new KdNode[size];
	x_axis = new uint32_t[size];
	y_axis = new uint32_t[size];
	z_axis = new uint32_t[size];
	for (uint32_t i = 0; i < size; i++) {
		x_axis[i] = i;
		y_axis[i] = i;
		z_axis[i] = i;
	}
	compareX.model = model;
	compareY.model = model;
	compareZ.model = model;
	std::sort(x_axis, x_axis + size, compareX);
	std::sort(y_axis, y_axis + size, compareY);
	std::sort(z_axis, z_axis + size, compareZ);
	
	uint32_t* x_axis_ind = new uint32_t[size];
	uint32_t* y_axis_ind = new uint32_t[size];
	uint32_t* z_axis_ind = new uint32_t[size];
	for (uint32_t i = 0; i < size; i++) {
		x_axis_ind[x_axis[i]] = i;
		y_axis_ind[y_axis[i]] = i;
		z_axis_ind[z_axis[i]] = i;
	}
	uint32_t* buff1 = new uint32_t[size];
	uint32_t* buff2 = new uint32_t[size];
	root = _subnode_axis(treeArray, model, nullptr, x_axis, y_axis, z_axis, x_axis_ind, y_axis_ind, z_axis_ind, buff1, buff2, 0, size);

}

void KdTree::NearestSearchBenchmark1(uint32_t pointIndex, Point& result, uint32_t& expandedNodes)
{
	result = nearestSearch(&treeArray[pointIndex], model[pointIndex].position, false);
	expandedNodes = this->_expandedNodes;
}

void KdTree::NearestSearchBenchmark2(uint32_t pointIndex, Point& result, uint32_t& expandedNodes)
{
	result = nearestSearch2(&treeArray[pointIndex], model[pointIndex].position, false);
	expandedNodes = this->_expandedNodes;
}

void KdTree::NearestSearch(uint32_t pointIndex, Point& result)
{
	result = nearestSearch2(&treeArray[pointIndex], model[pointIndex].position, false);
}

void KdTree::NearestSearch(glm::vec3 position, Point& result) 
{
	result = nearestSearch2(root, position, true);
}

void KdTree::NearestKSearch(uint32_t pointIndex, unsigned int k, std::vector<Point>& result)
{
	nearestKSearch(&treeArray[pointIndex], model[pointIndex].position, k, false, result);
}

void KdTree::NearestKSearch(glm::vec3 position, unsigned int k, std::vector<Point>& result) 
{
	nearestKSearch(root, position, k, true, result);
}

void KdTree::MinAxis(glm::vec3 axis, uint32_t& resultPoint, float& resultValue)
{
	minAxis(axis, resultPoint, resultValue);
}

void KdTree::MaxAxis(glm::vec3 axis, uint32_t& resultPoint, float& resultValue)
{
	maxAxis(axis, resultPoint, resultValue);
}

void KdTree::RangeAxis(glm::vec3 axis, float from, float to, std::vector<uint32_t>& resultPoints)
{
	rangeAxis(axis, from, to, resultPoints);
}

Point KdTree::nearestSearch(const KdNode* start, vec3 position, bool includeStart) {
	priority_queue<NodeQuery> queue;
	if (includeStart) {
		queue.push(NodeQuery(start, start, start->bounds.Distance(position)));
	}
	else {
		queue.push(NodeQuery(start, start, -1));
	}

	float bestDist = std::numeric_limits<float>().max();
	const KdNode* best = nullptr;
	NodeQuery currentQ;
	float currentBound;
	_expandedNodes = 0;
	while (!queue.empty() && (currentBound = (currentQ = queue.top()).lowBound) < bestDist) {
		queue.pop();
		_expandedNodes++;
		const KdNode* cn = currentQ.node;
		const KdNode* pv = currentQ.previous;
		const KdNode* left = cn->left;
		const KdNode* right = cn->right;
		const KdNode* parent = cn->parent;
		float currentDist = Bounds::Distance(position, model[cn->pointInd].position);
		if (currentBound >= 0 && currentDist < bestDist) {
			bestDist = currentDist;
			best = cn;
		}

		if (left != nullptr && left != pv) {
			float leftBound = left->bounds.Distance(position);
			if (leftBound < bestDist) {
				queue.push(NodeQuery(left, cn, leftBound));
			}

		}
		if (right != nullptr && right != pv) {
			float rightBound = right->bounds.Distance(position);
			if (rightBound < bestDist) {
				queue.push(NodeQuery(right, cn, rightBound));
			}
		}
		if (parent != nullptr && parent != pv) {
			float parentBound = 0;
			if (cn == parent->left) {
				parentBound = parent->leftBounds.Distance(position);
			}
			else {
				parentBound = parent->rightBounds.Distance(position);
			}
			if (parentBound < bestDist) {
				queue.push(NodeQuery(parent, cn, parentBound));
			}

		}
	}
	if (best != nullptr) {
		return model[best->pointInd];
	}
	return Point();
}

Point KdTree::nearestSearch2(const KdNode* start, vec3 position, bool includeStart) {
	queue<NodeQuery> queue;
	if (includeStart) {
		queue.push(NodeQuery(start, start, start->bounds.Distance(position)));
	}
	else {
		queue.push(NodeQuery(start, start, -1));
	}

	float bestDist = std::numeric_limits<float>().max();
	const KdNode* best = nullptr;
	NodeQuery currentQ;
	float currentBound = 0;
	_expandedNodes = 0;
	while (!queue.empty()) {
		currentQ = queue.front();
		currentBound = currentQ.lowBound;
		queue.pop();
		_expandedNodes++;
		const KdNode* cn = currentQ.node;
		const KdNode* pv = currentQ.previous;
		const KdNode* left = cn->left;
		const KdNode* right = cn->right;
		const KdNode* parent = cn->parent;
		float currentDist = Bounds::Distance(position, model[cn->pointInd].position);
		if (currentBound >= 0 && currentDist < bestDist) {
			bestDist = currentDist;
			best = cn;
		}

		if (left != nullptr && left != pv) {
			float leftBound = left->bounds.Distance(position);
			if (leftBound < bestDist) {
				queue.push(NodeQuery(left, cn, leftBound));
			}

		}
		if (right != nullptr && right != pv) {
			float rightBound = right->bounds.Distance(position);
			if (rightBound < bestDist) {
				queue.push(NodeQuery(right, cn, rightBound));
			}
		}
		if (parent != nullptr && parent != pv) {
			float parentBound = 0;
			if (cn == parent->left) {
				parentBound = parent->leftBounds.Distance(position);
			}
			else {
				parentBound = parent->rightBounds.Distance(position);
			}
			if (parentBound < bestDist) {
				queue.push(NodeQuery(parent, cn, parentBound));
			}

		}
	}
	if (best != nullptr) {
		return model[best->pointInd];
	}
	return Point();
}

void KdTree::nearestKSearch(const KdNode* start, vec3 position, unsigned int k, bool includeStart, std::vector<Point>& result) {
	vector<NodeQuery> queue;
	vector<KResult> results;
	if (includeStart) {
		queue.push_back(NodeQuery(start, start, start->bounds.Distance(position)));
	}
	else {
		queue.push_back(NodeQuery(start, start, -1));
	}

	float worseKDist = std::numeric_limits<float>().max();
	NodeQuery currentQ;
	float currentBound;
	while (!queue.empty() && ((currentBound = (currentQ = queue.front()).lowBound) < worseKDist || results.size() < k)) {
		pop_heap(queue.begin(), queue.end());
		queue.pop_back();
		const KdNode* cn = currentQ.node;
		const KdNode* pv = currentQ.previous;
		const KdNode* left = cn->left;
		const KdNode* right = cn->right;
		const KdNode* parent = cn->parent;
		float currentDist = Bounds::Distance(position, model[cn->pointInd].position);
		if (currentBound >= 0) {
			if (results.size() < k) {
				results.push_back(KResult(cn->pointInd, currentDist));
				push_heap(results.begin(), results.end());
			}
			else {
				if (currentDist < worseKDist) {
					pop_heap(results.begin(), results.end());
					results.back().dist = currentDist;
					results.back().pointInd = cn->pointInd;
					push_heap(results.begin(), results.end());
					worseKDist = currentDist;
				}
			}
			
		}

		if (left != nullptr && left != pv) {
			float leftBound = left->bounds.Distance(position);
			if (leftBound < worseKDist || results.size() < k) {
				queue.push_back(NodeQuery(left, cn, leftBound));
				push_heap(queue.begin(), queue.end());
			}

		}
		if (right != nullptr && right != pv) {
			float rightBound = right->bounds.Distance(position);
			if (rightBound < worseKDist || results.size() < k) {
				queue.push_back(NodeQuery(right, cn, rightBound));
				push_heap(queue.begin(), queue.end());
			}
		}
		if (parent != nullptr && parent != pv) {
			float parentBound = 0;
			if (cn == parent->left) {
				parentBound = parent->leftBounds.Distance(position);
			}
			else {
				parentBound = parent->rightBounds.Distance(position);
			}
			if (parentBound < worseKDist || results.size() < k) {
				queue.push_back(NodeQuery(parent, cn, parentBound));
				push_heap(queue.begin(), queue.end());
			}
		}
	}
	sort_heap(results.begin(), results.end());
	for (unsigned int i = 0; i < results.size(); i++) {
		result.push_back(model[results[i].pointInd]);
	}
}

void KdTree::minAxis(glm::vec3 axis, uint32_t& resultPoint, float& resultValue) {
	axis = glm::normalize(axis);
	priority_queue<NodeQuery> queue;
	queue.push(NodeQuery(root, root, root->bounds.MinAxisValue(axis)));

	float bestValue = std::numeric_limits<float>().max();
	const KdNode* best = nullptr;
	NodeQuery currentQ;
	float currentBound;
	_expandedNodes = 0;
	while (!queue.empty() && (currentBound = (currentQ = queue.top()).lowBound) < bestValue) {
		queue.pop();
		_expandedNodes++;
		const KdNode* cn = currentQ.node;
		const KdNode* pv = currentQ.previous;
		const KdNode* left = cn->left;
		const KdNode* right = cn->right;
		const KdNode* parent = cn->parent;
		float currentValue = glm::dot(model[cn->pointInd].position, axis);
		if (currentValue < bestValue) {
			bestValue = currentValue;
			best = cn;
		}

		if (left != nullptr && left != pv) {
			float leftBound = left->bounds.MinAxisValue(axis);
			if (leftBound < bestValue) {
				queue.push(NodeQuery(left, cn, leftBound));
			}

		}
		if (right != nullptr && right != pv) {
			float rightBound = right->bounds.MinAxisValue(axis);
			if (rightBound < bestValue) {
				queue.push(NodeQuery(right, cn, rightBound));
			}
		}
		if (parent != nullptr && parent != pv) {
			float parentBound = 0;
			if (cn == parent->left) {
				parentBound = parent->leftBounds.MinAxisValue(axis);
			}
			else {
				parentBound = parent->rightBounds.MinAxisValue(axis);
			}
			if (parentBound < bestValue) {
				queue.push(NodeQuery(parent, cn, parentBound));
			}

		}
	}
	resultValue = bestValue;
	resultPoint = 0;
	if (best != nullptr) {
		resultPoint = best->pointInd;
	}
}

void KdTree::maxAxis(glm::vec3 axis, uint32_t& resultPoint, float& resultValue)
{
	axis = glm::normalize(axis);
	priority_queue<NodeQuery> queue;
	queue.push(NodeQuery(root, root, root->bounds.MaxAxisValue(axis)));

	float bestValue = std::numeric_limits<float>().min();
	const KdNode* best = nullptr;
	NodeQuery currentQ;
	float currentBound;
	_expandedNodes = 0;
	while (!queue.empty() && (currentBound = (currentQ = queue.top()).lowBound) > bestValue) {
		queue.pop();
		_expandedNodes++;
		const KdNode* cn = currentQ.node;
		const KdNode* pv = currentQ.previous;
		const KdNode* left = cn->left;
		const KdNode* right = cn->right;
		const KdNode* parent = cn->parent;
		float currentValue = glm::dot(model[cn->pointInd].position, axis);
		if (currentValue > bestValue) {
			bestValue = currentValue;
			best = cn;
		}

		if (left != nullptr && left != pv) {
			float leftBound = left->bounds.MaxAxisValue(axis);
			if (leftBound > bestValue) {
				queue.push(NodeQuery(left, cn, leftBound));
			}

		}
		if (right != nullptr && right != pv) {
			float rightBound = right->bounds.MaxAxisValue(axis);
			if (rightBound > bestValue) {
				queue.push(NodeQuery(right, cn, rightBound));
			}
		}
		if (parent != nullptr && parent != pv) {
			float parentBound = 0;
			if (cn == parent->left) {
				parentBound = parent->leftBounds.MaxAxisValue(axis);
			}
			else {
				parentBound = parent->rightBounds.MaxAxisValue(axis);
			}
			if (parentBound > bestValue) {
				queue.push(NodeQuery(parent, cn, parentBound));
			}

		}
	}
	resultValue = bestValue;
	resultPoint = 0;
	if (best != nullptr) {
		resultPoint = best->pointInd;
	}
}

void KdTree::rangeAxis(glm::vec3 axis, float from, float to, std::vector<uint32_t>& resultPoints)
{
	axis = glm::normalize(axis);
	queue<NodeQuery> queue;
	queue.push(NodeQuery(root, root, 0));
	NodeQuery currentQ;
	_expandedNodes = 0;
	while (!queue.empty()) {
		currentQ = queue.front();
		queue.pop();
		_expandedNodes++;
		const KdNode* cn = currentQ.node;
		const KdNode* pv = currentQ.previous;
		const KdNode* left = cn->left;
		const KdNode* right = cn->right;
		const KdNode* parent = cn->parent;
		
		if (Bounds::InsideAxisRange(model[cn->pointInd].position, axis, from, to)) {
			resultPoints.push_back(cn->pointInd);
		}

		if (left != nullptr && left != pv) {
			if (left->bounds.ContainsAxisRange(axis, from, to)) {
				queue.push(NodeQuery(left, cn, 0));
			}

		}
		if (right != nullptr && right != pv) {
			if (right->bounds.ContainsAxisRange(axis, from, to)) {
				queue.push(NodeQuery(right, cn, 0));
			}
		}
		if (parent != nullptr && parent != pv) {
			bool parentIn = 0;
			if (cn == parent->left) {
				parentIn = parent->leftBounds.ContainsAxisRange(axis, from, to);
			}
			else {
				parentIn = parent->rightBounds.ContainsAxisRange(axis, from, to);
			}
			if (parentIn) {
				queue.push(NodeQuery(parent, cn, 0));
			}

		}
	}
}