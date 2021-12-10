#pragma once
#include "renderer.h"
#include "camera.h"
#include "point_cloud.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class PointShader : public Renderer{
private:
	struct VertexData {
		vec3 position = vec3(0,0,0);
		vec3 color = vec3(0,0,0);
		vec3 normal = vec3(0,0,0);
		float size = 0;
	};

	std::vector<VertexData> vertexBuffer;
	
	PointCloud& pointcloud;
	Camera& camera;
	glm::mat4& modelT;
	float& pSizeT;
	ColorShade colorShade;

	static constexpr size_t SAMPLE_RES = 32;
	float footprint[SAMPLE_RES * SAMPLE_RES];

	GLuint vbo = 0, vao = 0, textureId = 0, programId = 0;

	static constexpr float AMBIENT_I = 0.2f;
	static constexpr float DIFFUSE_I = 0.7f;
	static constexpr float SPECULAR_I = 0.7f;
	static constexpr float SPECULAR_POW = 4.0f;

	void generateFootprint();
	void loadVertexData(bool realloc);
	void updateUniforms();

public:
	PointShader(PointCloud& pointcloud, Camera& camera, glm::mat4& modelT, float& pSizeT, ColorShade colorShade);
	~PointShader();
	void ChangeColorShade(ColorShade colorShade);
	void ReloadPointCloud();

	void Begin();
	void Draw();
};