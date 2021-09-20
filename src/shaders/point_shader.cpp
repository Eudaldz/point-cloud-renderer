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
	loadShaderProgram(vertexCode.c_str(), fragmentCode.c_str());
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, SAMPLE_RES, SAMPLE_RES, 0, GL_RED, GL_UNSIGNED_BYTE, &footprint[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glUseProgram(programId);
	glUniform1i(glGetUniformLocation(programId, "fpSample"), 0);
	glUseProgram(0);
}

void PointShader::LoadModel(PointCloud pc)
{
	float* points = new float[6 * pc.vn];
	pointCount = pc.vn;
	for (int i = 0; i < pointCount; i++) {
		Point p = pc.points[i];
		points[i * 6] = p.position.x;
		points[i * 6 + 1] = p.position.y;
		points[i * 6 + 2] = p.position.z;
		points[i * 6 + 3] = p.color.r;
		points[i * 6 + 4] = p.color.g;
		points[i * 6 + 5] = p.color.b;
	}

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * pointCount * 6u, points, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6u * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6u * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void PointShader::SetTransforms(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
{
	glUseProgram(programId);
	glUniformMatrix4fv(glGetUniformLocation(programId, "model"), 1, GL_FALSE, value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(programId, "view"), 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(programId, "projection"), 1, GL_FALSE, value_ptr(projection));
}

void PointShader::SetPointSize(float psize)
{
	glUseProgram(programId);
	glUniform1f(glGetUniformLocation(programId, "psize"), psize);
}

void PointShader::Draw()
{
	glUseProgram(programId);
	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glDrawArrays(GL_POINTS, 0, pointCount);
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
			float xoff = (float)i / (float)(SAMPLE_RES - 1);
			float yoff = (float)j / (float)(SAMPLE_RES - 1);
			vec2 v = vec2((-0.5f + xoff) * 2.0f, (-0.5f + yoff) * 2.0f);
			footprint[ind] = glm::length(v) <= 1 ? 255 : 0;
		}
	}
}