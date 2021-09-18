#pragma once
#include "shader_program.h"
#include "point_cloud.h"

class PointShader : public ShaderProgram
{
private:
	static constexpr size_t SAMPLE_RES = 32;
	uint8_t footprint[SAMPLE_RES * SAMPLE_RES];
	
	
	GLuint vbo, vao, textureId;
	size_t pointCount;

	void generateFootprint();

public:
	PointShader();
	void Start();
	void LoadModel(PointCloud pc);
	void SetTransforms(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
	void SetPointSize(float psize);
	void Draw();
	void End();
};