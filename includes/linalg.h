#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace LINALG {
	void Eigenvalues(const glm::highp_dmat3& matrix, double& eig1, double& eig2, double& eig3);
	void SmallestEigen(const glm::highp_dmat3& matrix, glm::highp_dvec3& eigenVector, double& eigenValue);
	void SurfaceFeatures(const std::vector<glm::vec3>& points, glm::vec3& normal, float& curvature);
}