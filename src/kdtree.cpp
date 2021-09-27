#include "kdtree.h"
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace glm;

namespace {
	struct KResult {
		float dist;
		size_t pointInd;

		KResult(): dist(0), pointInd(-1)
		{}

		KResult(size_t pind, float d): pointInd(pind), dist(d)
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

float inline KdTree::Bounds::Distance(glm::vec3 p1, glm::vec3 p2) {
	vec3 l = p1 - p2;
	return glm::dot(l, l);
}

KdTree::KdNode* KdTree::_subnode_axis(KdTree::KdNode* graphArray, Point* points, KdTree::KdNode* parent, size_t* p_axis, size_t* s_axis1, size_t* s_axis2,
	size_t* p_axis_ind, size_t* s_axis1_ind, size_t* s_axis2_ind,
	size_t* buff1, size_t* buff2,
	size_t from, size_t to)
{
	size_t mid = from + (to - from) / 2;
	size_t medianInd = p_axis[mid];
	KdNode* current = &graphArray[medianInd];
	current->parent = parent;
	current->pointInd = medianInd;
	current->bounds = Bounds(points[medianInd].position);
	current->leftBounds = Bounds(points[medianInd].position);
	current->rightBounds = Bounds(points[medianInd].position);
	size_t l1 = from, r1 = mid + 1, l2 = from, r2 = mid + 1;

	buff1[mid] = medianInd;
	buff2[mid] = medianInd;
	s_axis1_ind[medianInd] = mid;
	s_axis2_ind[medianInd] = mid;

	for (size_t i = from; i < to; i++) {
		size_t c1 = s_axis1[i];
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
		size_t c2 = s_axis2[i];
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
	x_axis = new size_t[size];
	y_axis = new size_t[size];
	z_axis = new size_t[size];
	for (size_t i = 0; i < size; i++) {
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

	/*cout << endl << "X SORT:" << endl << " | ";
	for (size_t i = 0; i < size; i++) {
		cout << x_axis[i] << " | ";
	}
	cout << endl << "Y SORT:" << endl << " | ";
	for (size_t i = 0; i < size; i++) {
		cout << y_axis[i] << " | ";
	}
	cout << endl << "Z SORT:" << endl << " | ";
	for (size_t i = 0; i < size; i++) {
		cout << z_axis[i] << " | ";
	}
	cout << endl;*/
	
	size_t* x_axis_ind = new size_t[size];
	size_t* y_axis_ind = new size_t[size];
	size_t* z_axis_ind = new size_t[size];
	for (size_t i = 0; i < size; i++) {
		x_axis_ind[x_axis[i]] = i;
		y_axis_ind[y_axis[i]] = i;
		z_axis_ind[z_axis[i]] = i;
	}
	size_t* buff1 = new size_t[size];
	size_t* buff2 = new size_t[size];
	//cout << "TREE CONSTRUCTION:" << endl;
	root = _subnode_axis(treeArray, model, nullptr, x_axis, y_axis, z_axis, x_axis_ind, y_axis_ind, z_axis_ind, buff1, buff2, 0, size);

	/*cout << endl << "FINAL PARTITION:" << endl;
	for (size_t i = 0; i < size; i++) {
		cout << " ( "<<x_axis[i]<<" "<<y_axis[i]<<" "<<z_axis[i]<<" ) |";
	}*/
}

void KdTree::NearestSearchBenchmark(size_t pointIndex, Point& result, size_t& expandedNodes) 
{
	result = nearestSearch(&treeArray[pointIndex], model[pointIndex].position, false);
	expandedNodes = this->_expandedNodes;
}

void KdTree::NearestSearch(size_t pointIndex, Point& result) 
{
	result = nearestSearch(&treeArray[pointIndex], model[pointIndex].position, false);
}

void KdTree::NearestSearch(glm::vec3 position, Point& result) 
{
	result = nearestSearch(root, position, true);
}

void KdTree::NearestKSearch(size_t pointIndex, unsigned int k, std::vector<Point>& result) 
{
	result = nearestKSearch(&treeArray[pointIndex], model[pointIndex].position, k, false);
}

void KdTree::NearestKSearch(glm::vec3 position, unsigned int k, std::vector<Point>& result) 
{
	result = nearestKSearch(root, position, k, true);
}

Point KdTree::nearestSearch(const KdNode* start, vec3 position, bool includeStart) {
	vector<NodeQuery> queue;
	if (includeStart) {
		queue.push_back(NodeQuery(start, start, start->bounds.Distance(position)));
	}
	else {
		queue.push_back(NodeQuery(start, start, -1));
	}
	
	float bestDist = std::numeric_limits<float>().max();
	const KdNode* best = nullptr;
	NodeQuery currentQ;
	float currentBound;
	_expandedNodes = 0;
	while (!queue.empty() && (currentBound = (currentQ = queue.front()).lowBound) < bestDist) {
		pop_heap(queue.begin(), queue.end());
		queue.pop_back();
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
				queue.push_back(NodeQuery(left, cn, leftBound));
				push_heap(queue.begin(), queue.end());
			}

		}
		if (right != nullptr && right != pv) {
			float rightBound = right->bounds.Distance(position);
			if (rightBound < bestDist) {
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
			if (parentBound < bestDist) {
				queue.push_back(NodeQuery(parent, cn, parentBound));
				push_heap(queue.begin(), queue.end());
			}

		}
	}
	if (best != nullptr) {
		return model[best->pointInd];
	}
	return Point();
}

vector<Point> KdTree::nearestKSearch(const KdNode* start, vec3 position, unsigned int k, bool includeStart) {
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
	vector<Point> r;
	for (unsigned int i = 0; i < results.size(); i++) {
		r.push_back(model[results[i].pointInd]);
	}
	return r;
}