#pragma once
#include "glad/glad.h"

class Scene
{
private:
	void checkCompileErrors(GLuint shader, const char* type);
	void checkLinkErrors(GLuint program);

protected:
	int loadShaderProgram(const char* vShaderCode, const char* fShaderCode);

public:
	virtual void Start() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Render() = 0;
	virtual void End() = 0;
};