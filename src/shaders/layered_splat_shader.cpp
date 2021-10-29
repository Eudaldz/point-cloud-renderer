#include "shaders/layered_splat_shader.h"
#include <glm/ext.hpp>
#include "resources.h"
#include <string>
#include "point_cloud.h"
#include <iostream>
#include <math.h>

//using namespace glm;

LayeredSplatShader::LayeredSplatShader()
{
	pointCount = 0;
	depthStep = 0;
	viewDir = glm::vec3(0, 0, 0);
	minP = 0;
}

void LayeredSplatShader::Start()
{
	splatter.Start();
}

void LayeredSplatShader::LoadModel(PointCloud* pc)
{
	this->pc = pc;
	pointCount = pc->vn;
	splatter.LoadModel(pc);
	calculateDepthStep();
	buffer.reserve(pointCount);
}

void LayeredSplatShader::SetTransforms(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
{
	splatter.SetTransforms(model, view, projection);
	viewDir.x = view[0][2];
	viewDir.y = view[1][2];
	viewDir.z = view[2][2];
}

void LayeredSplatShader::SetPointSizeTransform(float psizet)
{
	splatter.SetPointSizeTransform(psizet);
}

void LayeredSplatShader::Draw()
{
	uint32_t tmp;
	pc->tree.AxisMin(viewDir, tmp, minP);
	glm::vec3 minpos = pc->points[tmp].position;
	float d = glm::dot(viewDir, minpos);
	pc->tree.AxisMax(viewDir, tmp, maxP);
	float currentDepth = minP - depthStep/2.0f;
	float endDepth = maxP + depthStep / 2.0f;
	do {
		buffer.clear();
		pc->tree.AxisRange(viewDir, currentDepth, currentDepth + depthStep, buffer);
		if (buffer.size() > 0) {
			splatter.SetElements(&buffer[0], buffer.size());
			splatter.Draw();
		}
		currentDepth += depthStep;
	} while (currentDepth < endDepth);
}

void LayeredSplatShader::End()
{
	splatter.End();
}

void LayeredSplatShader::calculateDepthStep()
{
	depthStep = pc->averagePointDist;
}