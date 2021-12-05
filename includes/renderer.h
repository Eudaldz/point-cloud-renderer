#pragma once
#include <glm/glm.hpp>
#include "camera.h"
#include "point_cloud.h"

enum class ColorShade {
	WHITE, WHITE_SHADED, COLOR, COLOR_SHADED, NORMAL
};

class Renderer {
public:
	virtual ~Renderer() = 0;
	
	virtual void SetModel(const PointCloud* pc) = 0;
	
	virtual void ColorShade(ColorShade colorShade) = 0;
	virtual void PointSizeTransform(float psizet) = 0;
	virtual void Camera(const Camera& view) = 0;
	virtual void ModelTransform(const glm::mat4& model) = 0;
	
	virtual void Draw() = 0;
};