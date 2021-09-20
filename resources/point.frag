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


