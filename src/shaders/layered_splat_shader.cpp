#include "shaders/layered_splat_shader.h"
#include <glm/ext.hpp>
#include "resources.h"
#include <string>
#include "point_cloud.h"
#include <iostream>
#include <math.h>
#include <chrono>

//using namespace glm;
using namespace std::chrono;
using namespace std;

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
	high_resolution_clock::time_point begin;
	high_resolution_clock::time_point end;
	duration<double, std::milli> time_span;
	float tree_time = 0, draw_time = 0;
	uint32_t tmp;
	pc->tree.AxisMin(viewDir, tmp, minP);
	glm::vec3 minpos = pc->points[tmp].position;
	float d = glm::dot(viewDir, minpos);
	pc->tree.AxisMax(viewDir, tmp, maxP);
	float currentDepth = minP - depthStep/2.0f;
	float endDepth = maxP + depthStep / 2.0f;
	do {
		buffer.clear();
		begin = high_resolution_clock::now();
		pc->tree.AxisRange(viewDir, currentDepth, currentDepth + depthStep, buffer);
		end = high_resolution_clock::now();
		time_span = end - begin;
		tree_time += time_span.count();
		if (buffer.size() > 0) {
			begin = high_resolution_clock::now();
			splatter.SetElements(&buffer[0], buffer.size());
			splatter.Draw();
			end = high_resolution_clock::now();
			time_span = end - begin;
			draw_time += time_span.count();
		}
		currentDepth += depthStep;
	} while (currentDepth < endDepth);
	cout << "\nAxis Sort: " << tree_time << "  || Draw: " << draw_time << endl;
}

void LayeredSplatShader::End()
{
	splatter.End();
}

void LayeredSplatShader::calculateDepthStep()
{
	depthStep = pc->averagePointDist;
}