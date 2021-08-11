#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

out vec4 pColor;
out vec3 pPos;
out vec4 footprintCenter;
out float footprintSize;

//Geometric transformations
uniform mat4 view;
uniform mat4 P;

//Kernel Size
uniform float pRadius;

//Viewport
uniform float viewportWidth;
uniform float viewportHeight;

void main()
{
	vec3 point = (view * vec4(aPos, 1.0f)).xyz;
	pPos = point;
	vec3 normal = point.xy == vec2(0,0) ? vec3(0,1,0) : normalize(cross(point, vec3(0,0,1)));
	float centerDist = dot(point, point);
	float outlineDist = sqrt(centerDist - pRadius*pRadius );
	float thetaSin = pRadius / outlineDist;
	float thetaCos = centerDist / outlineDist;
	vec3 v = normalize(point);
	vec3 u = cross(normal, v);
	vec3 outlineP1 = (thetaCos*v - thetaSin*u)*outlineDist;
	vec3 outlineP2 = (thetaCos*v + thetaSin*u)*outlineDist;
	vec4 projP1 = P * vec4(outlineP1, 1.0f);
	vec4 projP2 = P * vec4(outlineP2, 1.0f);
	projP1 /= projP1.w;
	projP2 /= projP2.w;
	vec2 diagonalV = (projP1 - projP2).xy;
	footprintSize = ceil(max(abs(diagonalV.x*viewportWidth/2.0f), abs(diagonalV.y*viewportHeight/2.0f)));
	footprintCenter = (projP1 + projP2)/2.0f;
	gl_Position = footprintCenter;
	gl_PointSize = footprintSize;
}