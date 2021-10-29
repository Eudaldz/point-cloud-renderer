#version 330 core

in vec3 pColor;

out vec4 FragColor;

uniform sampler2D fpSample;

uniform float aLightIntensity;
uniform float dLightIntensity;
uniform vec3 lightDirection;

void main()
{
	float b = texture(fpSample, gl_PointCoord).r;
	vec4 c = vec4(pColor, 1.0f);
    if(b < 0.5f)discard;
	vec2 xy = gl_PointCoord*2;
	float z = sqrt(1-dot(xy,xy));
	vec3 pNormal = vec3(xy, z);
	FragColor = c*(aLightIntensity + abs(dot(pNormal, lightDirection))*dLightIntensity);
}
