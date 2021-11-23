#pragma once
#include "shader_program.h"
#include "point_cloud.h"

class PointNormalShadeShader : public ShaderProgram
{
private:
	static constexpr size_t SAMPLE_RES = 32;
	float footprint[SAMPLE_RES * SAMPLE_RES];


	GLuint vbo, vao, textureId, programId;
	GLuint pointCount;

	void generateFootprint();
	void setLightUniforms();

public:
	PointNormalShadeShader();
	void Start();
	void LoadModel(PointCloud* pc);
	void SetTransforms(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
	void SetCameraDir(const glm::mat4& viewDir);
	void SetPointSizeTransform(float psize);
	void Draw();
	void End();
};