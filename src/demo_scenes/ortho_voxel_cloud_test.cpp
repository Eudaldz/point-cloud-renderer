#include "demo_scenes/ortho_voxel_cloud_test.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <vector>

namespace
{
	float constexpr waveRate = 4.0f;
	vec3 botColor = vec3(130.0/255.0f, 29.0f/255.0f, 48.0f/255.0f);
	vec3 topColor = vec3(1.0f, 197.0f/255.0f, 200.0f/255.0f);
	float constexpr waveAmplitude = 0.25;

	vec3 GetTextureColor(float u, float v)
	{
		float constexpr pi = glm::pi<float>();
		float wave = glm::sin(u * 2 * pi * waveRate);
		vec3 tc = topColor * (wave * waveAmplitude + 1);
		vec3 bc = botColor * (wave * waveAmplitude + 1);
		return glm::mix(bc, tc, v);
	}

	const char* vShaderCode = R""""(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 pColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float pSize;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	gl_PointSize = pSize;
	pColor = aColor;
}
)"""";

	const char* fShaderCode = R""""(
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
)"""";
}

OrthoVoxelCloudTest::OrthoVoxelCloudTest()
{
	VBO = VAO = programId = 0;
	generateVoxels();
}

void OrthoVoxelCloudTest::generateVoxels()
{
	voxels = new float[SAMPLES * SAMPLES * 6u];
	for (int i = 0; i < SAMPLES; i++) {
		for (int j = 0; j < SAMPLES; j++) {
			int ind = (i * SAMPLES + j) * 6;
			float xoff = (float)i / (float)(SAMPLES - 1);
			float yoff = (float)j / (float)(SAMPLES - 1);
			float x = (-0.5f + xoff) * SQ_WIDTH;
			float y = (-0.5f + yoff) * SQ_HEIGHT;
			vec3 color = GetTextureColor(xoff, yoff);
			voxels[ind] = x;
			voxels[ind + 1] = y;
			voxels[ind + 2] = 0.0f;
			voxels[ind + 3] = color.r;
			voxels[ind + 4] = color.g;
			voxels[ind + 5] = color.b;
		}
	}

	for (int i = 0; i < SAMPLE_RES; i++) {
		for (int j = 0; j < SAMPLE_RES; j++) {
			int ind = i * SAMPLE_RES + j;
			float xoff = (float)i / (float)(SAMPLE_RES - 1);
			float yoff = (float)j / (float)(SAMPLE_RES - 1);
			vec2 v = vec2((-0.5f + xoff)*2.0f, (-0.5f + yoff)*2.0f);
			footprint[ind] = glm::length(v) <= 1 ? (unsigned char)255 : (unsigned char)0;
		}
	}

}

void OrthoVoxelCloudTest::loadModelData()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* SAMPLES * SAMPLES * 6u, voxels, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, SAMPLE_RES, SAMPLE_RES, 0, GL_RED, GL_UNSIGNED_BYTE, &footprint[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glUseProgram(programId);
	glUniform1i(glGetUniformLocation(programId, "fpSample"), 0);
	glUseProgram(0);
}

void OrthoVoxelCloudTest::Start()
{
	programId = loadShaderProgram(vShaderCode, fShaderCode);
	loadModelData();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	camera.position = vec3(0, 0, 2);
	camera.up = vec3(0, 1, 0);
	camera.lookAt = vec3(0, 0, 0);
	camera.viewSize = 5;
	camera.nearClip = 0.1f;
	camera.farClip = 20.0f;
	camera.projType = Camera::ORTHO;
	background.camera = &camera;
	background.Start();
	cont.camera = &camera;
	cont.moveSpeed = 2.0f;
	cont.Start();
}

void OrthoVoxelCloudTest::Update(float deltaTime)
{
	cont.Update(deltaTime);
	float worldSize = SQ_WIDTH / (float)SAMPLES;
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	voxelSize = worldSize / camera.viewSize * (float)viewport[3] - 1;
	//voxelSize = 5;
}

void OrthoVoxelCloudTest::Render()
{
	glDisable(GL_DEPTH_TEST);
	background.Render();
	glEnable(GL_DEPTH_TEST);

	glUseProgram(programId);
	mat4 view = camera.GetViewMatrix();
	mat4 projection = camera.GetProjMatrix();
	mat4 model = mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(programId, "model"), 1, GL_FALSE, value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(programId, "view"), 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(programId, "projection"), 1, GL_FALSE, value_ptr(projection));
	glUniform1f(glGetUniformLocation(programId, "pSize"), voxelSize);

	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glDrawArrays(GL_POINTS, 0, SAMPLES * SAMPLES);
}

void OrthoVoxelCloudTest::End()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(programId);
	glDeleteTextures(1, &textureId);
}