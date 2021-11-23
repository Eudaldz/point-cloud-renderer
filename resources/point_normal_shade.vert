#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in float aSize;

out vec3 pColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 viewDir;

uniform vec3 lightDir;
uniform float ambientI;
uniform float diffuseI;
uniform float specularI;
uniform float specularPower;

uniform float psizet;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	gl_PointSize = 2.0 * aSize * psizet;
	//vec3 h = -normalize(lightDir + (viewDir * vec4(aPos, 1.0f)).xyz);
	//float lambda = 1.0f-dot(h, aNormal);
	//float beta = specularPower / 16.0f;
	//float specularTerm = max(0,1.0f-beta*lambda);
	//specularTerm = pow(specularTerm, 16);
	float diffuseTerm = max(0,dot(-lightDir, aNormal));
	pColor = aColor * (ambientI + diffuseI*diffuseTerm);
	//pColor += vec3(1,1,1) * specularI*specularTerm;
}