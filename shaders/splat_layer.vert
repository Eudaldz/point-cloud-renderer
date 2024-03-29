#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in float aSize;

out vec4 pColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float psizet;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	gl_PointSize = 2.0 * aSize * psizet;
	pColor = aColor;
}