#pragma once
#pragma once
#include "shader_program.h"
#include "point_cloud.h"
#include "splat_shader.h"
#include <vector>
#include "point.h"
#include "axial_projections.h"

class FtbSplatShader : public ShaderProgram
{
private:
	// POINT CLOUD
	GLuint pointCount;
	PointCloud* pc;
	AxialProjections* ap;

	//FOOTPRINT
	static constexpr size_t SAMPLE_RES = 32;
	float footprint[SAMPLE_RES * SAMPLE_RES];
	static constexpr float LAMBDA = 0.2f;

	//LAYERED DATA
	glm::vec3 viewDir;
	float minP, maxP;
	std::vector<GLuint> buffer;

	//GL SHADER
	GLuint splatProgram;
	GLuint blendProgram;
	GLuint stencilProgram;
	GLuint backgroundProgram;
	
	//GL FRAMEBUFFER
	GLuint layerFramebuffer;
	GLuint textureColor;
	GLuint textureAlphaWeight;
	GLuint layerStencil;
	GLuint compositeFramebuffer;
	GLuint compositeColor;
	GLuint compositeStencil;

	//GL MODEL BUFFER
	GLuint quadVAO, quadVBO;
	GLuint vbo, vao, ebo, textureId;
	GLuint elementCount;

	int viewportHeight, viewportWidth;

	void generateFootprint();
	void generateShaderProgram();
	void generateFramebuffers();
	void generateModelBuffers();

	void setLayerElements(unsigned int* elements, unsigned int n);
	void drawLayer();

public:
	FtbSplatShader();
	void Start();
	void LoadModel(PointCloud* pc);
	void SetTransforms(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
	void SetPointSizeTransform(float psize);
	void Draw();
	void End();
};