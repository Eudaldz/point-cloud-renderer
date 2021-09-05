#version 330 core
out vec4 FragColor;

in vec4 pColor;
in vec3 pPos;
in vec4 footprintCenter;
in float footprintSize;

uniform sampler1D printSample;
uniform float pRadius;
uniform mat4 invP;

//Viewport
uniform float viewportWidth;
uniform float viewportHeight;

void main()
{
	vec2 pCoord = vec2(-1,-1) + 2*gl_PointCoord;
	vec2 rect = vec2(footprintSize/viewportWidth, footprintSize/viewportHeight);
	vec4 fragPoint = footprintCenter + vec4(pCoord, 0, 0);
	vec4 fragVector = invP*fragPoint;
	fragVector /= fragVector.w;
	vec3 ray = normalize(fragVector.xyz);
	ray *= dot(ray, pPos);
	float dist = distance(pPos, ray);
	float sampleParamater = (dist / pRadius);
	float fragValue = float(texture(printSample, sampleParamater));
	FragColor = vec4(pColor.xyz, fragValue);
}