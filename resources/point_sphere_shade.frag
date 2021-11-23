#version 330 core

in vec3 pColor;
in vec3 pPos;

out vec4 FragColor;

uniform sampler2D fpSample;
uniform sampler2D normalTex;

uniform mat4 viewDir;

uniform vec3 lightDir;
uniform float ambientI;
uniform float diffuseI;
uniform float specularI;
uniform float specularPower;

void main()
{
	float b = texture(fpSample, gl_PointCoord).r;
	vec4 c = vec4(pColor, 1.0f);
    if(b < 0.0f)discard;
	vec3 pNormal = texture(normalTex, gl_PointCoord).xyz;
	vec3 fPos = pPos + pNormal;
	float diffuseTerm = max(0,dot(-lightDir, pNormal));
	FragColor = vec4(pColor * (ambientI + diffuseI*diffuseTerm), 1.0f);
}
