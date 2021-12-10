#include "shaders/point_shader.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "shaderc.h"

using namespace glm;
using namespace std;

PointShader::PointShader(PointCloud& pointcloud, Camera& camera, glm::mat4& modelT, float& pSizeT, ColorShade colorShade): 
	pointcloud(pointcloud), camera(camera), modelT(modelT), pSizeT(pSizeT), colorShade(colorShade)
{
	generateFootprint();
	programId = ShaderC::LoadShaderProgram("point.vert", "point.frag");
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
	
	glUniform1f(glGetUniformLocation(programId, "ambientI"), AMBIENT_I);
	glUniform1f(glGetUniformLocation(programId, "diffuseI"), DIFFUSE_I);
	glUniform1f(glGetUniformLocation(programId, "specularI"), SPECULAR_I);
	glUniform1f(glGetUniformLocation(programId, "specularPower"), SPECULAR_POW);

	vec3 lightDir = glm::normalize(vec3(0, -1, -1));
	glUniform3f(glGetUniformLocation(programId, "lightDir"), 
		lightDir.x, lightDir.y, lightDir.z);
	glUseProgram(0);
	
	loadVertexData(false);
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
			footprint[ind] = f + 1.0f / SAMPLE_RES;
		}
	}
}

void PointShader::loadVertexData(bool realloc)
{
	const vector<Point>& pc = pointcloud.Points();
	vertexBuffer.clear();
	if (!realloc)vertexBuffer.reserve(pc.size());
	for (uint32_t i = 0; i < pc.size(); i++) {
		Point p = pc[i];
		VertexData vd;
		vd.position = p.position;
		vd.size = p.pointSize;
		switch (colorShade.color) {
		case ColorMode::WHITE:
			vd.color = vec3(1, 1, 1);
			break;
		case ColorMode::COLOR:
			vd.color = p.color;
			break;
		case ColorMode::LABEL:
			vec3 lc(0.2f, 0.2f, 0.2f);
			if (p.label < MAX_LABELS) {
				lc = COLOR_LABEL[p.label];
			}
			vd.color = lc;
			break;
		case ColorMode::PRED:
			vec3 pc(0.2f, 0.2f, 0.2f);
			if (p.part < MAX_LABELS) {
				pc = COLOR_LABEL[p.part];
			}
			vd.color = pc;
			break;
		case ColorMode::NORMAL:
			vd.color = glm::abs(p.normal);
			break;
		case ColorMode::CURVATURE:
			vd.color = vec3(1, 1, 1) * p.curvature / 10.0f;
			break;
		}
		if (colorShade.shade == ShadeMode::SHADE) {
			vd.normal = p.normal;
		}
		vertexBuffer.push_back(vd);
	}

	if (realloc) {
		glBindVertexArray(vao);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VertexData) * vertexBuffer.size(), &vertexBuffer[0]);
	}
	else {
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * vertexBuffer.size(), &vertexBuffer[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(9 * sizeof(float)));
		glEnableVertexAttribArray(3);

		glBindVertexArray(0);
	}
}

void PointShader::ChangeColorShade(ColorShade colorShade) 
{
	this->colorShade = colorShade;
	loadVertexData(true);
}

void PointShader::ReloadPointCloud()
{
	loadVertexData(true);
}

void PointShader::Begin()
{
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
	glUseProgram(programId);
	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
}

void PointShader::Draw()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	updateUniforms();
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_POINTS, 0, vertexBuffer.size());
}

void inline PointShader::updateUniforms()
{
	mat4 view = camera.GetViewMatrix();
	mat4 projection = camera.GetProjMatrix();
	mat4 viewDir = camera.GetViewDirMatrix();
	glUniformMatrix4fv(glGetUniformLocation(programId, "model"), 1, GL_FALSE, value_ptr(modelT));
	glUniformMatrix4fv(glGetUniformLocation(programId, "view"), 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(programId, "projection"), 1, GL_FALSE, value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(programId, "viewDir"), 1, GL_FALSE, value_ptr(viewDir));
	glUniform1f(glGetUniformLocation(programId, "psizet"), pSizeT);
	if (colorShade.shade == ShadeMode::SHADE) {
		glUniform1i(glGetUniformLocation(programId, "shading"), true);
	}
	else {
		glUniform1i(glGetUniformLocation(programId, "shading"), false);
	}
}

PointShader::~PointShader() 
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(programId);
	glDeleteTextures(1, &textureId);
}