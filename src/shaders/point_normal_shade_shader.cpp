#include "shaders/point_normal_shade_shader.h"
#include <glm/ext.hpp>
#include "resources.h"
#include <string>
#include "point_cloud.h"
#include <iostream>

using namespace glm;

PointNormalShadeShader::PointNormalShadeShader()
{
	pointCount = 0;
	vao = 0;
	vbo = 0;
	textureId = 0;
}

void PointNormalShadeShader::Start()
{
	generateFootprint();
	std::string vertexCode = Resources::GetResourceText("point_normal_shade.vert");
	std::string fragmentCode = Resources::GetResourceText("point_normal_shade.frag");
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
	setLightUniforms();
	glUseProgram(0);
}

void PointNormalShadeShader::LoadModel(PointCloud* pc)
{
	float* points = new float[10 * pc->vn];
	pointCount = pc->vn;
	for (unsigned int i = 0; i < pointCount; i++) {
		Point p = pc->points[i];
		points[i * 10] = p.position.x;
		points[i * 10 + 1] = p.position.y;
		points[i * 10 + 2] = p.position.z;
		points[i * 10 + 3] = p.color.r;
		points[i * 10 + 4] = p.color.g;
		points[i * 10 + 5] = p.color.b;
		points[i * 10 + 6] = p.normal.x;
		points[i * 10 + 7] = p.normal.y;
		points[i * 10 + 8] = p.normal.z;
		points[i * 10 + 9] = p.pointSize;
	}

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * pointCount * 10u, points, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10u * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10u * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10u * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 10u * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);
}

void PointNormalShadeShader::SetTransforms(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
{
	glUseProgram(programId);
	glUniformMatrix4fv(glGetUniformLocation(programId, "model"), 1, GL_FALSE, value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(programId, "view"), 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(programId, "projection"), 1, GL_FALSE, value_ptr(projection));
}

void PointNormalShadeShader::SetCameraDir(const glm::mat4& viewDir)
{
	glUseProgram(programId);
	glUniformMatrix4fv(glGetUniformLocation(programId, "viewDir"), 1, GL_FALSE, value_ptr(viewDir));
}

void PointNormalShadeShader::SetPointSizeTransform(float psizet)
{
	glUseProgram(programId);
	glUniform1f(glGetUniformLocation(programId, "psizet"), psizet);
}

void PointNormalShadeShader::setLightUniforms() 
{
	glUseProgram(programId);
	glUniform1f(glGetUniformLocation(programId, "ambientI"), 0.2);
	glUniform1f(glGetUniformLocation(programId, "diffuseI"), 0.7);
	glUniform1f(glGetUniformLocation(programId, "specularI"), 0.3);
	glUniform1f(glGetUniformLocation(programId, "specularPower"), 4);
	
	vec3 lightDir = glm::normalize(vec3(0, -1, -1));
	glUniform3f(glGetUniformLocation(programId, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
}

void PointNormalShadeShader::Draw()
{
	glEnable(GL_DEPTH_TEST);
	glUseProgram(programId);
	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glDrawArrays(GL_POINTS, 0, pointCount);
	glDisable(GL_DEPTH_TEST);
}

void PointNormalShadeShader::End()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteProgram(programId);
	glDeleteTextures(1, &textureId);
}


void PointNormalShadeShader::generateFootprint()
{
	for (int i = 0; i < SAMPLE_RES; i++) {
		for (int j = 0; j < SAMPLE_RES; j++) {
			int ind = i * SAMPLE_RES + j;
			float xoff = (float)j / (float)(SAMPLE_RES - 1);
			float yoff = (float)i / (float)(SAMPLE_RES - 1);
			vec2 v = vec2((-0.5f + xoff) * 2.0f, (-0.5f + yoff) * 2.0f);
			float f = glm::max(0.0f, 1.0f - glm::length(v));
			footprint[ind] = f + 1.0f / SAMPLE_RES;
		}
	}
}