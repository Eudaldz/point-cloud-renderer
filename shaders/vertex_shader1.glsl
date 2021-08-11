#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

out vec4 Color;

//Geometric transformations
uniform mat4 view;
uniform mat4 P;
uniform mat4x3 planeP;

//Kernel Size
uniform float kernelSize;

//Viewport
uniform float width;
uniform float height;

mat4 sphereSymMatrix(vec3 center, float radius);
vec3 solve_quadratic_eq(float a, float b, float c);

void main()
{
	vec4 point = view * vec4(aPos, 1.0f);
	vec4 focus = P*point;
	float depth = focus.z/focus.w;
	mat4 Q = sphereSymMatrix(point.xyz, kernelSize);
	mat4 invQ = inverse(Q);
	mat3 invC = planeP * Q * transpose(planeP);
	mat3 C = inverse(invC);
	mat2 A = mat2(C[0][0], C[1][0], C[0][1], C[1][1]);
	float k = -determinant(C)/determinant(A);
	vec4 proj_focus =  vec4(invC[2][0]/invC[2][2], invC[2][1]/invC[2][2], depth, 1.0f);
	vec3 evalues = solve_quadratic_eq(1, A[0][0]+A[1][1], A[0][0]*A[1][1] - A[1][0]*A[0][1]);
	float l1 = evalues.x/k, l2 = evalues.y/k;
	float l = l1 > 0 && l2 > 0 ? sqrt( max(l1,l2) ) : 0;
	vec2 dir = l*normalize(focus.xy);
	float sizeX = dir.x * width;
	float sizeY = dir.y * height;

	gl_PointSize = ceil(max(dir.x, dir.y));
	gl_Position = proj_focus;
}

mat4 sphereSymMatrix(vec3 center, float radius){
	float c = center.x*center.x + center.y*center.y + center.z*center.z - radius*radius;
	return mat4(1,0,0,-center.x,
				0,1,0,-center.y,
				0,0,1,-center.z,
				-center.x, -center.y, -center.z, c);
}

vec3 solve_quadratic_eq(float a, float b, float c){
	float d = b*b - 4*a*c;
	vec3 result;
	result.z = d < 0 ? 0 : 1;
	float sq = sqrt(d);
	result.x = (-b + sq)/(2*a);
	result.y = (-b - sq)/(2*a);
	return result;
}