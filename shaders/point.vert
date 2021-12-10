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
uniform bool shading;

uniform float psizet;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	gl_PointSize = 2.0 * aSize * psizet;
	if(shading){
		vec3 pos = (model * vec4(aPos, 1.0f)).xyz;
		vec3 normal = normalize((model*vec4(aNormal, 0.0f)).xyz);
		vec3 viewV = normalize((viewDir * vec4(pos, 1.0f)).xyz);
		float d = dot(viewV, normal);
		if(d > 0)normal = -normal;
		float diffuseTerm = max(0, dot(-lightDir, normal));
		pColor = aColor * (ambientI + diffuseI*diffuseTerm);
	}else{
		pColor = aColor;
	}
}