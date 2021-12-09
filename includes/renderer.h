#pragma once
#include <glm/glm.hpp>
#include "camera.h"
#include "point_cloud.h"

enum class ColorShade {
	WHITE, WHITE_SHADED, COLOR, COLOR_SHADED, NORMAL, CURVATURE
};

class Renderer {
public:
	virtual ~Renderer() = 0;
	
	virtual void ChangeColorShade(ColorShade colorShade) = 0;
	virtual void ReloadPointCloud() = 0;
	
	virtual void Begin() = 0;
	virtual void Draw() = 0;
	virtual void End() = 0;
};