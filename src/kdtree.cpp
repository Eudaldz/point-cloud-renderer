#include "kdtree.h"
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace glm;

KdTree::Bounds::Bounds() : 
	minX(neg_inf), minY(neg_inf), minZ(neg_inf), maxX(pos_inf), maxY(pos_inf), maxZ(pos_inf) 
{}

KdTree::Bounds::Bounds(glm::vec3 p) : 
	minX(p.x), maxX(p.x), minY(p.y), maxY(p.y), minZ(p.z), maxZ(p.z)
{}

KdTree::Bounds::Bounds(Axis ax, Partition part, float offset) : 
	minX(neg_inf), minY(neg_inf), minZ(neg_inf), maxX(pos_inf), maxY(pos_inf), maxZ(pos_inf)
{
	if (ax == Axis::XAxis) {
		if (part == Partition::Left)maxX = offset;
		else minX = offset;
	}
	else if (ax == Axis::YAxis) {
		if (part == Partition::Left)maxY = offset;
		else minY = offset;
	}
	else if (ax == Axis::ZAxis) {
		if (part == Partition::Left)maxZ = offset;
		else minZ = offset;
	}
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

void KdTree::Bounds::Unite(Bounds b)
{
	minX = std::min(minX, b.minX);
	minY = std::min(minY, b.minY);
	minZ = std::min(minZ, b.minZ);
	maxX = std::max(maxX, b.maxX);
	maxY = std::max(maxY, b.maxY);
	maxZ = std::max(maxZ, b.maxZ);
}

void KdTree::Bounds::Intersect(Bounds b)
{
	minX = std::max(minX, b.minX);
	minY = std::max(minY, b.minY);
	minZ = std::max(minZ, b.minZ);
	maxX = std::min(maxX, b.maxX);
	maxY = std::min(maxY, b.maxY);
	maxZ = std::min(maxZ, b.maxZ);
	if (minX > maxX || minY > maxY || minZ > maxZ) {
		cout << "error";
	}
}

float inline KdTree::Bounds::OutsideDistance(glm::vec3 p) const
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

float inline KdTree::Bounds::InsideDistance(glm::vec3 p) const
{
	float dx = 0;
	float dy = 0;
	float dz = 0;
	dx = std::min(p.x - minX, maxX - p.x);
	dy = std::min(p.y - minY, maxY - p.y);
	dz = std::min(p.z - minZ, maxZ - p.z);
	return std::max(std::min(dx*dx, std::min(dy*dy, dz*dz)), 0.0f);
}

float inline KdTree::Bounds::AxisMinValue(glm::vec3 v) const
{
	float dx = v.x >= 0 ? minX : maxX;
	float dy = v.y >= 0 ? minY : maxY;
	float dz = v.z >= 0 ? minZ : maxZ;
	return glm::dot(v, vec3(dx, dy, dz));
}

float inline KdTree::Bounds::AxisMaxValue(glm::vec3 v) const
{
	float dx = v.x >= 0 ? maxX : minX;
	float dy = v.y >= 0 ? maxY : minY;
	float dz = v.z >= 0 ? maxZ : minZ;
	return glm::dot(v, vec3(dx, dy, dz));
}

bool inline KdTree::Bounds::ContainsAxisRange(glm::vec3 axis, float from, float to) const
{
	float minv = AxisMinValue(axis);
	float maxv = AxisMaxValue(axis);
	return !(minv >= to || maxv < from);
}

float inline KdTree::Bounds::Distance(glm::vec3 p1, glm::vec3 p2) {
	vec3 l = p1 - p2;
	return glm::dot(l, l);
}

float inline KdTree::Bounds::AxisValue(glm::vec3 axis, glm::vec3 point) {
	return glm::dot(axis, point);
}

bool inline KdTree::Bounds::InsideAxisRange(glm::vec3 point, glm::vec3 axis, float from, float to) {
	float d = glm::dot(point, axis);
	return d >= from && d < to;
}

KdTree::KdNode* KdTree::_subnode_axis(KdTree::KdNode* graphArray, Point* points, KdTree::KdNode* parent, uint32_t* p_axis, uint32_t* s_axis1, uint32_t* s_axis2,
	uint32_t* p_axis_ind, uint32_t* s_axis1_ind, uint32_t* s_axis2_ind,
	uint32_t* buff1, uint32_t* buff2,
	uint32_t from, uint32_t to,
	Bounds maxBounds, KdTree::Bounds::Axis ax)
{
	uint32_t mid = from + (to - from) / 2;
	uint32_t medianInd = p_axis[mid];
	KdNode* current = &graphArray[medianInd];
	current->parent = parent;
	current->pointInd = medianInd;
	vec3 pos = points[medianInd].position;
	current->minBounds = Bounds(pos);
	current->maxBounds = maxBounds;
	float partitionOffset;
	KdTree::Bounds::Axis nextAxis;
	if (ax == Bounds::Axis::XAxis) {
		partitionOffset = pos.x;
		nextAxis = Bounds::Axis::YAxis;
	}else if (ax == Bounds::Axis::YAxis) {
		partitionOffset = pos.y;
		nextAxis = Bounds::Axis::ZAxis;
	}
	else {
		partitionOffset = pos.z;
		nextAxis = Bounds::Axis::XAxis;
	}
	Bounds leftMaxBounds(ax, Bounds::Partition::Left, partitionOffset);
	Bounds rightMaxBounds(ax, Bounds::Partition::Right, partitionOffset);
	leftMaxBounds.Intersect(maxBounds);
	rightMaxBounds.Intersect(maxBounds);
	
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

	current->left = nullptr;
	if (from < mid) {
		current->left = _subnode_axis(graphArray, points, current, s_axis1, s_axis2, p_axis,
			s_axis1_ind, s_axis2_ind, p_axis_ind, buff1, buff2, from, mid, leftMaxBounds, nextAxis);
		current->minBounds.Unite(current->left->minBounds);
	}
	current->right = nullptr;
	if (mid + 1 < to) {
		current->right = _subnode_axis(graphArray, points, current, s_axis1, s_axis2, p_axis,
			s_axis1_ind, s_axis2_ind, p_axis_ind, buff1, buff2, mid + 1, to, rightMaxBounds, nextAxis);
		current->minBounds.Unite(current->right->minBounds);
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
	_expandedNodes = 0;
	compareX.model = nullptr;
	compareY.model = nullptr;
	compareZ.model = nullptr;
	root = nullptr;
	x_axis_ind = nullptr;
	y_axis_ind = nullptr;
	z_axis_ind = nullptr;
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
	root = _subnode_axis(treeArray, model, nullptr, x_axis, y_axis, z_axis, 
		x_axis_ind, y_axis_ind, z_axis_ind, 
		buff1, buff2, 0, size, 
		Bounds(), Bounds::Axis::XAxis);

	searchQueue.Reserve(32);
	minSearchQueue.Reserve(32);
	maxSearchQueue.Reserve(32);
}

void KdTree::NearestSearch(uint32_t pointIndex, uint32_t& result)
{
	if (pointIndex >= size) { return; }
	searchQueue.Clear();
	float bestDist = std::numeric_limits<float>().max();
	KdNode* best = nullptr;
	KdNode* current = &treeArray[pointIndex];
	vec3 position = model[current->pointInd].position;
	searchQueue.Push(NodeSearch(current, nullptr, -1));
	_expandedNodes = 0;
	while (!searchQueue.Empty()) {
		_expandedNodes++;
		NodeSearch s = searchQueue.Front();
		searchQueue.Pop();
		float searchValue = s.searchValue;
		current = s.node;
		float currentDist = Bounds::Distance(position, model[current->pointInd].position);
		if (searchValue >= 0 && currentDist < bestDist) {
			bestDist = currentDist;
			best = current;
		}
		KdNode* prev = s.previous;
		KdNode* left = current->left;
		KdNode* right = current->right;
		KdNode* parent = current->parent;
		if (left != nullptr && left != prev) {
			if (left->minBounds.OutsideDistance(position) < bestDist) {
				searchQueue.Push(NodeSearch(left, current, 1));
			}
		}
		if (right != nullptr && right != prev) {
			if (right->minBounds.OutsideDistance(position) < bestDist) {
				searchQueue.Push(NodeSearch(right, current, 1));
			}
		}
		if (parent != nullptr && parent != prev) {
			if (current->maxBounds.InsideDistance(position) < bestDist) {
				searchQueue.Push(NodeSearch(parent, current, 1));
			}
		}
	}
	if (best != nullptr) {
		result = best->pointInd;
	}
	else {
		result = pointIndex;
	}
}

void KdTree::NearestSearch(glm::vec3 position, uint32_t& result) 
{
	if (size == 0) { return; }
	minSearchQueue.Clear();
	float bestDist = std::numeric_limits<float>().max();
	KdNode* best = nullptr;
	KdNode* current = root;
	minSearchQueue.Push(NodeSearch(current, nullptr, 0));
	_expandedNodes = 0;
	while (!minSearchQueue.Empty()) {
		_expandedNodes++;
		NodeSearch s = minSearchQueue.Front();
		minSearchQueue.Pop();
		current = s.node;
		float currentDist = Bounds::Distance(position, model[current->pointInd].position);
		if (currentDist < bestDist) {
			bestDist = currentDist;
			best = current;
		}
		KdNode* left = current->left;
		KdNode* right = current->right;
		if (left != nullptr) {
			float minDist = left->minBounds.OutsideDistance(position);
			if (minDist < bestDist) {
				minSearchQueue.Push(NodeSearch(left, current, minDist));
			}
		}
		if (right != nullptr) {
			float minDist = right->minBounds.OutsideDistance(position);
			if (minDist < bestDist) {
				minSearchQueue.Push(NodeSearch(right, current, minDist));
			}
		}
	}
	if(best!=nullptr)result = best->pointInd;
}

void KdTree::NearestKSearch(uint32_t pointIndex, unsigned int k, std::vector<uint32_t>& result)
{
	if (pointIndex >= size) { return; }
	searchQueue.Clear();
	resultsQueue.Clear();
	float worstKDist = std::numeric_limits<float>().max();
	KdNode* current = &treeArray[pointIndex];
	vec3 position = model[current->pointInd].position;
	searchQueue.Push(NodeSearch(current, nullptr, -1));
	_expandedNodes = 0;
	while (!searchQueue.Empty()) {
		_expandedNodes++;
		NodeSearch s = searchQueue.Front();
		searchQueue.Pop();
		float searchValue = s.searchValue;
		current = s.node;
		float currentDist = Bounds::Distance(position, model[current->pointInd].position);
		if (searchValue >= 0) {
			if (resultsQueue.Size() < k) {
				resultsQueue.Push(KNResult(current->pointInd, currentDist));
				if(resultsQueue.Size() == k)worstKDist = resultsQueue.Front().dist;
			}
			else if (currentDist < worstKDist) {
				resultsQueue.Pop();
				resultsQueue.Push(KNResult(current->pointInd, currentDist));
				worstKDist = resultsQueue.Front().dist;
			}
		}
		KdNode* prev = s.previous;
		KdNode* left = current->left;
		KdNode* right = current->right;
		KdNode* parent = current->parent;
		if (left != nullptr && left != prev) {
			if (left->minBounds.OutsideDistance(position) < worstKDist) {
				searchQueue.Push(NodeSearch(left, current, 1));
			}
		}
		if (right != nullptr && right != prev) {
			if (right->minBounds.OutsideDistance(position) < worstKDist) {
				searchQueue.Push(NodeSearch(right, current, 1));
			}
		}
		if (parent != nullptr && parent != prev) {
			if (current->maxBounds.InsideDistance(position) < worstKDist) {
				searchQueue.Push(NodeSearch(parent, current, 1));
			}
		}
	}
	unsigned int s = (unsigned int)resultsQueue.Size();
	result.clear();
	result.resize(s);
	for (unsigned int i = 0; i < s; i++) {
		result[s-1-i] = resultsQueue.Front().pointInd;
		resultsQueue.Pop();
	}
}

void KdTree::NearestKSearch(glm::vec3 position, unsigned int k, std::vector<uint32_t>& result) 
{
	if (size == 0) { return; }
	minSearchQueue.Clear();
	resultsQueue.Clear();
	float worstKDist = std::numeric_limits<float>().max();
	KdNode* current = root;
	minSearchQueue.Push(NodeSearch(current, nullptr, 0));
	_expandedNodes = 0;
	while (!minSearchQueue.Empty()) {
		_expandedNodes++;
		NodeSearch s = minSearchQueue.Front();
		minSearchQueue.Pop();
		current = s.node;
		float currentDist = Bounds::Distance(position, model[current->pointInd].position);
		if (resultsQueue.Size() < k) {
			resultsQueue.Push(KNResult(current->pointInd, currentDist));
			if (resultsQueue.Size() == k)worstKDist = resultsQueue.Front().dist;
		}
		else if (currentDist < worstKDist) {
			resultsQueue.Pop();
			resultsQueue.Push(KNResult(current->pointInd, currentDist));
			worstKDist = resultsQueue.Front().dist;
		}
		KdNode* left = current->left;
		KdNode* right = current->right;
		if (left != nullptr) {
			float minDist = left->minBounds.OutsideDistance(position);
			if (minDist < worstKDist) {
				minSearchQueue.Push(NodeSearch(left, current, minDist));
			}
		}
		if (right != nullptr) {
			float minDist = right->minBounds.OutsideDistance(position);
			if (minDist < worstKDist) {
				minSearchQueue.Push(NodeSearch(right, current, minDist));
			}
		}
	}
	unsigned int s = (unsigned int)resultsQueue.Size();
	result.clear();
	result.resize(s);
	for (unsigned int i = 0; i < s; i++) {
		result[s - 1 - i] = resultsQueue.Front().pointInd;
		resultsQueue.Pop();
	}
}

void KdTree::AxisMin(glm::vec3 axis, uint32_t& resultPoint, float& resultValue)
{
	if (size == 0) { return; }
	axis = glm::normalize(axis);
	minSearchQueue.Clear();
	float minValue = std::numeric_limits<float>().max();
	KdNode* best = nullptr;
	KdNode* current = root;
	minSearchQueue.Push(NodeSearch(current, nullptr, 0));
	_expandedNodes = 0;
	while (!minSearchQueue.Empty()) {
		_expandedNodes++;
		NodeSearch s = minSearchQueue.Front();
		minSearchQueue.Pop();
		current = s.node;
		float currentValue = Bounds::AxisValue(axis, model[current->pointInd].position);
		if (currentValue < minValue) {
			minValue = currentValue;
			best = current;
		}
		KdNode* left = current->left;
		KdNode* right = current->right;
		if (left != nullptr) {
			float leftMinValue = left->minBounds.AxisMinValue(axis);
			if (leftMinValue < minValue) {
				minSearchQueue.Push(NodeSearch(left, current, leftMinValue));
			}
		}
		if (right != nullptr) {
			float rightMinValue = right->minBounds.AxisMinValue(axis);
			if (rightMinValue < minValue) {
				minSearchQueue.Push(NodeSearch(right, current, rightMinValue));
			}
		}
	}
	if (best != nullptr) {
		resultPoint = best->pointInd;
		resultValue = minValue;
	}
}

void KdTree::AxisMax(glm::vec3 axis, uint32_t& resultPoint, float& resultValue)
{
	if (size == 0) { return; }
	axis = glm::normalize(axis);
	maxSearchQueue.Clear();
	float maxValue = -std::numeric_limits<float>().max();
	KdNode* best = nullptr;
	KdNode* current = root;
	maxSearchQueue.Push(NodeSearch(current, nullptr, 0));
	_expandedNodes = 0;
	while (!maxSearchQueue.Empty()) {
		_expandedNodes++;
		NodeSearch s = maxSearchQueue.Front();
		maxSearchQueue.Pop();
		current = s.node;
		float currentValue = Bounds::AxisValue(axis, model[current->pointInd].position);
		if (currentValue > maxValue) {
			maxValue = currentValue;
			best = current;
		}
		KdNode* left = current->left;
		KdNode* right = current->right;
		if (left != nullptr) {
			float leftMaxValue = left->minBounds.AxisMaxValue(axis);
			if (leftMaxValue > maxValue) {
				maxSearchQueue.Push(NodeSearch(left, current, leftMaxValue));
			}
		}
		if (right != nullptr) {
			float rightMaxValue = right->minBounds.AxisMaxValue(axis);
			if (rightMaxValue > maxValue) {
				maxSearchQueue.Push(NodeSearch(right, current, rightMaxValue));
			}
		}
	}
	if (best != nullptr) {
		resultPoint = best->pointInd;
		resultValue = maxValue;
	}
}

void KdTree::AxisRange(glm::vec3 axis, float from, float to, std::vector<uint32_t>& resultPoints)
{
	if (size == 0) { return; }
	axis = glm::normalize(axis);
	resultPoints.clear();
	searchQueue.Clear();
	KdNode* current = root;
	searchQueue.Push(NodeSearch(current, nullptr, 0));
	_expandedNodes = 0;
	while (!searchQueue.Empty()) {
		_expandedNodes++;
		NodeSearch s = searchQueue.Front();
		searchQueue.Pop();
		current = s.node;
		if (Bounds::InsideAxisRange(model[current->pointInd].position, axis, from, to)) {
			resultPoints.push_back(current->pointInd);
		}
		KdNode* left = current->left;
		KdNode* right = current->right;
		if (left != nullptr) {
			if (left->minBounds.ContainsAxisRange(axis, from, to)) {
				searchQueue.Push(NodeSearch(left, current, 0));
			}
		}
		if (right != nullptr) {
			if (right->minBounds.ContainsAxisRange(axis, from, to)) {
				searchQueue.Push(NodeSearch(right, current, 0));
			}
		}
	}
}