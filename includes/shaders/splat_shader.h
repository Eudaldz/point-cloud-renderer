#pragma once
#include "shader_program.h"
#include "point_cloud.h"

class SplatShader : public ShaderProgram
{
private:
	static constexpr size_t SAMPLE_RES = 32;
	float footprint[SAMPLE_RES * SAMPLE_RES];
	static constexpr float LAMBDA = 0.2f;

	GLuint splatProgram;
	GLuint blendProgram;
	GLuint vbo, vao, ebo, textureId;
	GLuint quadVAO, quadVBO;
	GLuint framebuffer;
	GLuint textureColorSum;
	GLuint textureColorWeight;
	GLuint pointCount;
	GLuint elementCount;

	int viewportHeight, viewportWidth;

	void generateFootprint();
	void generateBufferTextures();
public:
	SplatShader();
	void Start();
	void LoadModel(PointCloud* pc);
	void SetElements(unsigned int* elements, unsigned int n);
	void SetTransforms(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
	void SetPointSizeTransform(float psize);
	void Draw();
	void End();
};