#include "resources.h"
#include "string"
#include "demo_data.h"

using namespace std;

namespace
{
	const char* vs_point = R""""(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 pColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float pSize;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	gl_PointSize = pSize;
	pColor = aColor;
}
)"""";

	const char* fs_point = R""""(
#version 330 core

in vec3 pColor;

out vec4 FragColor;

uniform sampler2D fpSample;

void main()
{
	float b = texture(fpSample, gl_PointCoord).r;
	vec4 c = vec4(pColor, 1.0f);
    if(b < 0.5f)discard;
	FragColor = c;
}
)"""";

	const char* demoData;
}

ShaderCode Resource::getShaderCode(ShaderName sn)
{
	ShaderCode sc;
	switch (sn) {
	case Point:
		sc.vertex = vs_point;
		sc.fragment = fs_point;
		break;
	}
}

const char* Resource::getPCData(DataName dn)
{
	switch (dn) {
	case Demo1:
		return demo_pc;
	}
}