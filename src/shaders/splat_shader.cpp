#include "shaders/splat_shader.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "resources.h"
#include "point_cloud.h"

using namespace glm;

SplatShader::SplatShader()
{
	pointCount = 0;
	vao = 0;
	vbo = 0;
	textureId = 0;
	ebo = 0;
	elementCount = 0;
}

void SplatShader::Start()
{
	generateFootprint();
	std::string vertexCode = Resources::GetResourceText("splat_layer.vert");
	std::string fragmentCode = Resources::GetResourceText("splat_layer.frag");
	splatProgram = loadShaderProgram(vertexCode.c_str(), fragmentCode.c_str());
	vertexCode = Resources::GetResourceText("splat_blend.vert");
	fragmentCode = Resources::GetResourceText("splat_blend.frag");
	blendProgram = loadShaderProgram(vertexCode.c_str(), fragmentCode.c_str());
	
	glGenFramebuffers(1, &framebuffer);
	glGenTextures(1, &textureColor);
	glGenTextures(1, &textureAlphaWeight);
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	viewportWidth = viewport[2];
	viewportHeight = viewport[3];
	generateBufferTextures();

	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, SAMPLE_RES, SAMPLE_RES, 0, GL_RED, GL_FLOAT, &footprint[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glUseProgram(splatProgram);
	glUniform1i(glGetUniformLocation(splatProgram, "fpSample"), 0);
	glUseProgram(0);

	glUseProgram(blendProgram);
	glUniform1i(glGetUniformLocation(blendProgram, "ColorSum"), 0);
	glUniform1i(glGetUniformLocation(blendProgram, "AlphaWeightSum"), 1);
	glUseProgram(0);
}

void SplatShader::LoadModel(PointCloud* pc)
{
	float* points = new float[8 * pc->vn];
	pointCount = pc->vn;
	unsigned int* indices = new unsigned int[pointCount];
	elementCount = pointCount;
	for (uint32_t i = 0; i < pointCount; i++) {
		Point p = pc->points[i];
		points[i * 8] = p.position.x;
		points[i * 8 + 1] = p.position.y;
		points[i * 8 + 2] = p.position.z;
		points[i * 8 + 3] = p.color.r;
		points[i * 8 + 4] = p.color.g;
		points[i * 8 + 5] = p.color.b;
		points[i * 8 + 6] = p.color.a;
		points[i * 8 + 7] = p.pointSize;
		indices[i] = i;
	}

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * pointCount * 8u, points, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * pointCount, indices, GL_STREAM_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8u * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8u * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 8u * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void SplatShader::SetElements(unsigned int* elements, unsigned int n)
{
	elementCount = n;
	glBindVertexArray(vao);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * n, elements);
	glBindVertexArray(0);
}

void SplatShader::SetTransforms(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
{
	glUseProgram(splatProgram);
	glUniformMatrix4fv(glGetUniformLocation(splatProgram, "model"), 1, GL_FALSE, value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(splatProgram, "view"), 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(splatProgram, "projection"), 1, GL_FALSE, value_ptr(projection));
}

void SplatShader::SetPointSizeTransform(float psizet)
{
	glUseProgram(splatProgram);
	glUniform1f(glGetUniformLocation(splatProgram, "psizet"), psizet);
}

void SplatShader::Draw()
{
	glEnable(GL_BLEND);
	
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glBlendFunc(GL_ONE, GL_ONE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(splatProgram);
	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glDrawElements(GL_POINTS, elementCount, GL_UNSIGNED_INT, 0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(blendProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureColor);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureAlphaWeight);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_BLEND);
}

void SplatShader::End()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteProgram(splatProgram);
	glDeleteProgram(blendProgram);
	glDeleteTextures(1, &textureId);
}

void SplatShader::generateFootprint()
{
	for (int i = 0; i < SAMPLE_RES; i++) {
		for (int j = 0; j < SAMPLE_RES; j++) {
			int ind = i * SAMPLE_RES + j;
			float xoff = (float)j / (float)(SAMPLE_RES - 1);
			float yoff = (float)i / (float)(SAMPLE_RES - 1);
			vec2 v = vec2((-0.5f + xoff) * 2.0f, (-0.5f + yoff) * 2.0f);
			float x = glm::dot(v, v);
			footprint[ind] = glm::exp(-(x * x) / LAMBDA);
		}
	}
}

void SplatShader::generateBufferTextures()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	
	glBindTexture(GL_TEXTURE_2D, textureColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, viewportWidth, viewportHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, textureAlphaWeight);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, viewportWidth, viewportHeight, 0, GL_RG, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColor, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textureAlphaWeight, 0);
	
	const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, buffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}