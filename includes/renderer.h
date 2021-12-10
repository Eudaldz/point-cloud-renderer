#pragma once
#include <glm/glm.hpp>
#include "camera.h"
#include "point_cloud.h"

using namespace glm;
enum class ColorMode {
	WHITE, COLOR, LABEL, PRED, NORMAL, CURVATURE
};

enum class ShadeMode {
	NO_SHADE, SHADE
};

struct ColorShade {
	ColorMode color;
	ShadeMode shade;
	ColorShade(ColorMode c, ShadeMode s):color(c), shade(s){}
};

class Renderer {
public:
	static constexpr int MAX_LABELS = 10;
	static constexpr glm::vec3 COLOR_LABEL[MAX_LABELS] = {
		vec3(0.75f,0.75f,0.75f),
		vec3(1,0.5f, 0), vec3(1,0,0.5f),
		vec3(0,1,0.5f), vec3(0.5f,1,0),
		vec3(0,0.5f,1), vec3(0.5f,0,1),
		vec3(1,0.25f,0.25f), vec3(0.25f,1,0.25f), vec3(0.25f,0.25f,1)
	};
	virtual ~Renderer() = 0 {};
	
	virtual void ChangeColorShade(ColorShade colorShade) = 0;
	virtual void ReloadPointCloud() = 0;
	
	virtual void Begin() = 0;
	virtual void Draw() = 0;
};