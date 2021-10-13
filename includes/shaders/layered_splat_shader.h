#pragma once
#include "shader_program.h"
#include "point_cloud.h"
#include "splat_shader.h"
#include <vector>
#include "point.h"

class LayeredSplatShader : public ShaderProgram
{
private:
	GLuint pointCount;
	float depthStep;
	PointCloud* pc;

	SplatShader splatter;
	glm::vec3 viewDir;
	float minP, maxP;
	std::vector<GLuint> buffer;

	void calculateDepthStep();

public:
	LayeredSplatShader();
	void Start();
	void LoadModel(PointCloud* pc);
	void SetTransforms(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
	void SetPointSizeTransform(float psize);
	void Draw();
	void End();
};