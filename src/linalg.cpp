#include "linalg.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>
#include <iostream>

using namespace glm;
using namespace std;

namespace {
	
	double inline pow2(double x) { return x * x; }
	
	void inline sort(double& x1, double& x2, double& x3) {
		//TODO: USE SWAP FUNCTION IDIOT!
		double t = x3;;
		if (x1 > x2 && x1 > x3) {
			x3 = x1;
			x1 = x2;
			x2 = t;
		}
		else if (x2 > x3) {
			x3 = x2;
			x2 = t;
		}
		t = x2;
		if (x1 > x2) {
			x2 = x1;
			x1 = t;
		}
	}
}

void LINALG::Eigenvalues(const highp_dmat3& m, double& eig1, double& eig2, double& eig3) {
	//ASSUME MATRIX IS SYMMETRIC
	double m1 = m[0][0], m2 = m[1][1], m3 = m[2][2];
	double m12 = m[0][1], m13 = m[0][2], m23 = m[1][2];

	if (m12 == 0 && m13 == 0 && m23 == 0) {
		eig1 = m1;
		eig2 = m2;
		eig3 = m3;
		sort(eig3, eig2, eig1);
	}
	else {
		double q = (m1 + m2 + m3) / 3.0;
		double p1 = m12 * m12 + m13 * m13 + m23 * m23;
		double p2 = pow2(m1 - q) + pow2(m2 - q) + pow2(m3 - q) + 2 * p1;
		double p = sqrt(p2 / 6.0);
		highp_dmat3 B = (1.0 / p) * (m - q * highp_dmat3(1.0));
		double r = glm::determinant(B) / 2.0;
		if (r < -1)r = -1;
		else if (r > 1)r = 1;
		double phi = glm::acos(r) / 3.0;
		constexpr double pi = glm::pi<double>();
		eig1 = q + 2 * p * glm::cos(phi);
		eig3 = q + 2 * p * glm::cos(phi + (2 * pi / 3.0));
		eig2 = 3 * q - eig1 - eig3;
	}
}

void LINALG::SmallestEigen(const highp_dmat3& m, highp_dvec3& eigenVector, double& eigenValue) {
	//ASSUME MATRIX IS SYMMETRIC
	double eig1, eig2, eig3;
	Eigenvalues(m, eig1, eig2, eig3);
	eigenValue = eig3;
	highp_dmat3 d1 = m - eig1 * highp_dmat3(1.0);
	highp_dmat3 d2 = m - eig2 * highp_dmat3(1.0);
	highp_dmat3 d3 = m - eig3 * highp_dmat3(1.0);

	highp_dmat3 me1 = d2 * d3;
	highp_dmat3 me2 = d1 * d3;
	highp_dmat3 me3 = d1 * d2;

	highp_dvec3 c1 = me3[0];
	highp_dvec3 c2 = me3[1];
	highp_dvec3 c3 = me3[2];
	
	double l1 = glm::length(c1);
	double l2 = glm::length(c2);
	double l3 = glm::length(c3);
	
	if (l1 >= l2 && l1 >= l3) {
		eigenVector = c1;
	}
	else if (l2 >= l3) {
		eigenVector = c2;
	}
	else {
		eigenVector = c3;
	}
}

void LINALG::SurfaceFeatures(const vector<vec3>& points, vec3& normal, float& curvature) {
	if (points.size() < 3) {
		normal = vec3(0, 0, 0);
		curvature = 0;
	}
	
	highp_dvec3 center(0, 0, 0);
	for (uint32_t i = 0; i < points.size(); i++) {
		center += points[i];
	}
	center /= points.size();
	highp_dmat3 matrix;
	for (uint32_t i = 0; i < 3; i++) {
		for (uint32_t j = 0; j < 3; j++) {
			double cov = 0;
			for (uint32_t n = 0; n < points.size(); n++) {
				cov += ((double)points[n][i] - center[i]) * ((double)points[n][j] - center[j]);
			}
			matrix[j][i] = cov;
		}
	}
	double trace = matrix[0][0] + matrix[1][1] + matrix[2][2];
	if (trace != 0) {
		highp_dvec3 eigVector;
		double eigValue;
		SmallestEigen(matrix, eigVector, eigValue);
		normal = glm::normalize(eigVector);
		curvature = (float)abs(eigValue / trace);
	}
	else {
		normal = vec3(0, 0, 0);
		curvature = 0;
	}
}