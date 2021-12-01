#include "shaders/ftb_splat_shader.h"
#include <glm/ext.hpp>
#include "resources.h"
#include <string>
#include "point_cloud.h"
#include <iostream>
#include <math.h>
#include <chrono>

using namespace glm;
using namespace std::chrono;
using namespace std;

FtbSplatShader::FtbSplatShader()
{
	pointCount = 0;
	viewDir = glm::vec3(0, 0, 0);
	minP = 0;
}

void FtbSplatShader::Start()
{
	generateFootprint();
	generateShaderProgram();
	generateFramebuffers();
	generateModelBuffers();
}

void FtbSplatShader::LoadModel(PointCloud* pc)
{
	this->pc = pc;
	this->ap = &pc->axialProjections;
	ap->ConstructSlices(pc->averagePointDist * 0.75f);
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
	buffer.reserve(pointCount);
}

void FtbSplatShader::setLayerElements(unsigned int* elements, unsigned int n)
{
	elementCount = n;
	glBindVertexArray(vao);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * n, elements);
	glBindVertexArray(0);
}

void FtbSplatShader::SetTransforms(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
{
	glUseProgram(splatProgram);
	glUniformMatrix4fv(glGetUniformLocation(splatProgram, "model"), 1, GL_FALSE, value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(splatProgram, "view"), 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(splatProgram, "projection"), 1, GL_FALSE, value_ptr(projection));
	viewDir.x = -view[0][2];
	viewDir.y = -view[1][2];
	viewDir.z = -view[2][2];
}

void FtbSplatShader::SetPointSizeTransform(float psizet)
{
	glUseProgram(splatProgram);
	glUniform1f(glGetUniformLocation(splatProgram, "psizet"), psizet);
}

void FtbSplatShader::Draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, compositeFramebuffer);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glStencilMask(0xFF);
	glClearStencil(0);
	glClear(GL_STENCIL_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, layerFramebuffer);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glStencilMask(0xFF);
	glClearStencil(0);
	glClear(GL_STENCIL_BUFFER_BIT);

	glEnable(GL_BLEND);
	glEnable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	
	uint32_t tmp;
	AxSlices slices = ap->GetSlices(viewDir);

	const std::vector<ProjectionSlice> *vslice = slices.slices;
	bool reversed = slices.reversed;
	if (!vslice->empty()) {
		uint32_t begin = 0;
		uint32_t end = vslice->size();
		if (reversed) {
			begin = end-1;
			end = -1;
		}
		while (begin != end) {
			ProjectionSlice ps = (*vslice)[begin];
			if (ps.count > 0) {
				setLayerElements(&(ps.buffer[0]), ps.count);
				drawLayer();
			}
			if (slices.reversed) {
				--begin;
			}
			else {
				++begin;
			}
		}
	}

	

	glDisable(GL_STENCIL_TEST);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBlendFunc(GL_ONE, GL_SRC_ALPHA);
	glUseProgram(backgroundProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, compositeColor);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glDisable(GL_BLEND);
}

void FtbSplatShader::drawLayer()
{
	//glDisable(GL_STENCIL_TEST);
	//step 1: draw individual layer & write stencil
	glBindFramebuffer(GL_FRAMEBUFFER, layerFramebuffer);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glStencilFunc(GL_NOTEQUAL, 0xFF, 0xF0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0x0F);
	glBlendFunc(GL_ONE, GL_ONE);
	glUseProgram(splatProgram);
	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glDrawElements(GL_POINTS, elementCount, GL_UNSIGNED_INT, 0);
	
	//step 2: sync stencil
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, compositeFramebuffer);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, layerFramebuffer);
	glBlitFramebuffer(0, 0, viewportWidth, viewportHeight,
		0, 0, viewportWidth, viewportHeight,
		GL_STENCIL_BUFFER_BIT, GL_NEAREST);

	//step 3: compose
	glBindFramebuffer(GL_FRAMEBUFFER, compositeFramebuffer);
	glStencilFunc(GL_EQUAL, 0xFF, 0x0F);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilMask(0x00);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(blendProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureColor);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureAlphaWeight);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//step 3: write stencil
	glStencilFunc(GL_EQUAL, 0xFF, 0x0F);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0xF0);
	glUseProgram(stencilProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, compositeColor);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//step 4: clear stencil
	glStencilMask(0x0F);
	glClearStencil(0);
	glClear(GL_STENCIL_BUFFER_BIT);
	
	//step 5: sync stencil
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, layerFramebuffer);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, compositeFramebuffer);
	glBlitFramebuffer(0, 0, viewportWidth, viewportHeight,
		0, 0, viewportWidth, viewportHeight,
		GL_STENCIL_BUFFER_BIT, GL_NEAREST);
}

void FtbSplatShader::End()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteProgram(splatProgram);
	glDeleteProgram(blendProgram);
	glDeleteTextures(1, &textureId);
}

void FtbSplatShader::generateFootprint()
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

void FtbSplatShader::generateShaderProgram()
{
	std::string vertexCode = Resources::GetResourceText("splat_layer.vert");
	std::string fragmentCode = Resources::GetResourceText("splat_layer.frag");
	splatProgram = loadShaderProgram(vertexCode.c_str(), fragmentCode.c_str());
	vertexCode = Resources::GetResourceText("splat_blend.vert");
	fragmentCode = Resources::GetResourceText("splat_blend2.frag");
	blendProgram = loadShaderProgram(vertexCode.c_str(), fragmentCode.c_str());
	vertexCode = Resources::GetResourceText("splat_stencil.vert");
	fragmentCode = Resources::GetResourceText("splat_stencil.frag");
	stencilProgram = loadShaderProgram(vertexCode.c_str(), fragmentCode.c_str());
	vertexCode = Resources::GetResourceText("texture_blend.vert");
	fragmentCode = Resources::GetResourceText("texture_blend.frag");
	backgroundProgram = loadShaderProgram(vertexCode.c_str(), fragmentCode.c_str());
}

void FtbSplatShader::generateFramebuffers()
{
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	viewportWidth = viewport[2];
	viewportHeight = viewport[3];
	
	//layer frame buffer
	glGenFramebuffers(1, &layerFramebuffer);
	glGenTextures(1, &textureColor);
	glGenTextures(1, &textureAlphaWeight);
	glBindFramebuffer(GL_FRAMEBUFFER, layerFramebuffer);

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

	glGenRenderbuffers(1, &layerStencil);
	glBindRenderbuffer(GL_RENDERBUFFER, layerStencil);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, viewportWidth, viewportHeight);
	glFramebufferRenderbuffer(
		GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, layerStencil
	);

	const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, buffers);
	
	//composite frame buffer
	glGenFramebuffers(1, &compositeFramebuffer);
	glGenTextures(1, &compositeColor);
	glBindFramebuffer(GL_FRAMEBUFFER, compositeFramebuffer);

	glBindTexture(GL_TEXTURE_2D, compositeColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, viewportWidth, viewportHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, compositeColor, 0);

	glGenRenderbuffers(1, &compositeStencil);
	glBindRenderbuffer(GL_RENDERBUFFER, compositeStencil);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, viewportWidth, viewportHeight);
	glFramebufferRenderbuffer(
		GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, compositeStencil
	);

	const GLenum buffers2[] = { GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, buffers2);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FtbSplatShader::generateModelBuffers()
{
	//QUAD MODEL
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

	//POINTCLOUD MODEL
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	
	//FOOTPRINT TEXTURE
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, SAMPLE_RES, SAMPLE_RES, 0, GL_RED, GL_FLOAT, &footprint[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//SET TEXTURE PARAMATER INDEX
	glUseProgram(splatProgram);
	glUniform1i(glGetUniformLocation(splatProgram, "fpSample"), 0);
	glUseProgram(0);

	glUseProgram(blendProgram);
	glUniform1i(glGetUniformLocation(blendProgram, "ColorSum"), 0);
	glUniform1i(glGetUniformLocation(blendProgram, "AlphaWeightSum"), 1);
	glUseProgram(0);
}