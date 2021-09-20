#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 pColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float psize;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	gl_PointSize = psize;
	pColor = aColor;
}