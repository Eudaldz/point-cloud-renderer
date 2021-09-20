#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 0) in vec3 center;
layout (location = 1) in vec4 aColor;


out vec4 pColor;
out vec3 pPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float pRadius;

void main()
{
	vec4 point = (view * model * vec4(aPos, 1.0f)); // 32
	pPos = point.xyz; // 1 
	float dist2 = dot(pPos, pPos); // 1
	float inv_dist = inversesqrt(dist2); // 1
	float d_ = sqrt(pRadius*pRadius - dist2); // 7
	float r = pRadius * d_ * inv_dist; // 3
	//vec4 p_ = projection * point; // 16
	//float iw = 1.0f / p_.w; // 5
	//float z = p_.z * iw; // 1
	//vec2 xy = p_.xy * iw; // 2

}