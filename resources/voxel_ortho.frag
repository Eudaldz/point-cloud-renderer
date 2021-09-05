#version 330 core

in vec4 pColor;

out vec4 FragColor;

uniform usampler2D fpSample;

void main()
{
	uint b = texture(fpSample, gl_PointCoord).r;
	if(b == 0U)discard;
	FragColor = pColor;
}


