#include "shaders/splat_shader.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "resources.h"

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
	glGenTextures(1, &textureColorSum);
	glGenTextures(1, &textureColorWeight);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, SAMPLE_RES, SAMPLE_RES, 0, GL_RED, GL_FLOAT, &footprint[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glUseProgram(splatProgram);
	glUniform1i(glGetUniformLocation(splatProgram, "fpSample"), 0);
	glUseProgram(0);

	glUseProgram(blendProgram);
	glUniform1i(glGetUniformLocation(blendProgram, "colorSum"), 0);
	glUniform1i(glGetUniformLocation(blendProgram, "colorWeight"), 1);
	glUseProgram(0);
}

void SplatShader::LoadModel(PointCloud* pc)
{
	float* points = new float[8 * pc->vn];
	pointCount = pc->vn;
	unsigned int* indices = new unsigned int[pointCount];
	elementCount = pointCount;
	for (size_t i = 0; i < pointCount; i++) {
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
	glBindTexture(GL_TEXTURE_2D, textureColorSum);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureColorWeight);
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
			float xoff = (float)i / (float)(SAMPLE_RES - 1);
			float yoff = (float)j / (float)(SAMPLE_RES - 1);
			vec2 v = vec2((-0.5f + xoff) * 2.0f, (-0.5f + yoff) * 2.0f);
			float x = glm::dot(v, v);
			footprint[ind] = glm::exp(-(x * x) / LAMBDA);
		}
	}
}

void SplatShader::generateBufferTextures()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	
	glBindTexture(GL_TEXTURE_2D, textureColorSum);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, viewportWidth, viewportHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, textureColorWeight);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, viewportWidth, viewportHeight, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorSum, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textureColorWeight, 0);
	
	const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, buffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}