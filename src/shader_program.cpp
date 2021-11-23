#include "shader_program.h"
#include "shaders/point_shader.h"
#include "shaders/splat_shader.h"
#include "shaders/layered_splat_shader.h"
#include "shaders/ftb_splat_shader.h"
#include "shaders/point_normal_shade_shader.h"
#include "shaders/point_sphere_shade_shader.h"
#include <iostream>


void ShaderProgram::checkCompileErrors(GLuint shader, const char* type)
{
    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION::" << type << "::" << typeid(*this).name() << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }
}

void ShaderProgram::checkLinkErrors(GLuint program)
{
    GLint success;
    GLchar infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        std::cout << "ERROR::PROGRAM_LINKING::" << typeid(*this).name() << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }
}

GLuint ShaderProgram::loadShaderProgram(const char* vShaderCode, const char* fShaderCode)
{
    GLuint vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertex);
    glAttachShader(programId, fragment);
    glLinkProgram(programId);
    checkLinkErrors(programId);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return programId;
}

ShaderProgram* ShaderProgram::NewShader(ShaderName sn)
{
    switch (sn) {
    case ShaderName::Point:
        return new PointShader();
    case ShaderName::PointNormalShade:
        return new PointNormalShadeShader();
    case ShaderName::PointSphereShade:
        return new PointSphereShadeShader();
    case ShaderName::Splat:
        return new LayeredSplatShader();
    case ShaderName::FtbSplat:
        return new FtbSplatShader();
    default:
        throw "Shader not yet implemented!";
    }
}