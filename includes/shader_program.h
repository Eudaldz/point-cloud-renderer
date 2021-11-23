#pragma once
#include <glm/glm.hpp>
#include "glad/glad.h"
#include "point_cloud.h"

enum class ShaderName
{
	Point, PointNormalShade, PointSphereShade, Splat, FtbSplat
};

class ShaderProgram
{
private:
	void checkCompileErrors(GLuint shader, const char* type);
	void checkLinkErrors(GLuint program);

protected:
	GLuint loadShaderProgram(const char* vShaderCode, const char* fShaderCode);

public:
	virtual void Start() = 0;
	virtual void LoadModel(PointCloud* pc) = 0;
	virtual void SetTransforms(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) = 0;
	virtual void SetCameraWorldView(const glm::mat4& worldView) {};
	virtual void SetCameraDir(const glm::mat4& viewDir) {};
	virtual void SetPointSizeTransform(float psize) = 0;
	virtual void Draw() = 0;
	virtual void End() = 0;
	static ShaderProgram* NewShader(ShaderName sn);
};