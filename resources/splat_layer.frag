#version 330 core

in vec4 pColor;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out float FragWeight;

uniform sampler2D fpSample;

void main()
{
	float b = texture(fpSample, gl_PointCoord).r;
	FragColor = b*pColor;
	FragWeight = b;
}