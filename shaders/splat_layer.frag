#version 330 core

in vec4 pColor;

layout(location = 0) out vec3 FragColor;
layout(location = 1) out vec2 FragAlphaWeight;

uniform sampler2D fpSample;

void main()
{
	float b = texture(fpSample, gl_PointCoord).r;
	FragColor = pColor.rgb * pColor.a * b;
	FragAlphaWeight.r = pColor.a * b;
	FragAlphaWeight.g = b;
}