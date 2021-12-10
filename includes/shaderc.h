#pragma once
#include <glad/glad.h>
#include <string>

namespace ShaderC {
	GLuint LoadShaderProgram(std::string vertexRes, std::string fragmentRes);
}