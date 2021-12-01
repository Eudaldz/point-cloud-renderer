#include "shaders/point_shader.h"
#include <glm/ext.hpp>
#include "resources.h"
#include <string>
#include "point_cloud.h"
#include <iostream>

using namespace glm;

PointShader::PointShader()
{
	pointCount = 0;
	vao = 0;
	vbo = 0;
	textureId = 0;
}

void PointShader::Start()
{
	generateFootprint();
	std::string vertexCode = Resources::GetResourceText("point.vert");
	std::string fragmentCode = Resources::GetResourceText("point.frag");
	programId = loadShaderProgram(vertexCode.c_str(), fragmentCode.c_str());
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, SAMPLE_RES, SAMPLE_RES, 0, GL_RED, GL_FLOAT, &footprint[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glUseProgram(programId);
	glUniform1i(glGetUniformLocation(programId, "fpSample"), 0);
	glUseProgram(0);
}

void PointShader::LoadModel(PointCloud* pc)
{
	float* points = new float[7 * pc->vn];
	pointCount = pc->vn;
	for (unsigned int i = 0; i < pointCount; i++) {
		Point p = pc->points[i];
		points[i * 7] = p.position.x;
		points[i * 7 + 1] = p.position.y;
		points[i * 7 + 2] = p.position.z;
		points[i * 7 + 3] = p.color.r;
		points[i * 7 + 4] = p.color.g;
		points[i * 7 + 5] = p.color.b;
		points[i * 7 + 6] = p.pointSize;
	}

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * pointCount * 7u, points, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7u * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7u * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7u * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void PointShader::SetTransforms(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
{
	glUseProgram(programId);
	glUniformMatrix4fv(glGetUniformLocation(programId, "model"), 1, GL_FALSE, value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(programId, "view"), 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(programId, "projection"), 1, GL_FALSE, value_ptr(projection));
}

void PointShader::SetPointSizeTransform(float psizet)
{
	glUseProgram(programId);
	glUniform1f(glGetUniformLocation(programId, "psizet"), psizet);
}

void PointShader::Draw()
{
	glEnable(GL_DEPTH_TEST);
	glUseProgram(programId);
	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glDrawArrays(GL_POINTS, 0, pointCount);
	glDisable(GL_DEPTH_TEST);
}

void PointShader::End()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteProgram(programId);
	glDeleteTextures(1, &textureId);
}


void PointShader::generateFootprint()
{
	for (int i = 0; i < SAMPLE_RES; i++) {
		for (int j = 0; j < SAMPLE_RES; j++) {
			int ind = i * SAMPLE_RES + j;
			float xoff = (float)j / (float)(SAMPLE_RES - 1);
			float yoff = (float)i / (float)(SAMPLE_RES - 1);
			vec2 v = vec2((-0.5f + xoff) * 2.0f, (-0.5f + yoff) * 2.0f);
			float f = glm::max(0.0f, 1.0f - glm::length(v));
			footprint[ind] = f + 1.0f/SAMPLE_RES;
		}
	}
}