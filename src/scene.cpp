#include "scene.h"
#include "glad/glad.h"
#include <iostream>

void Scene::checkCompileErrors(GLuint shader, const char* type)
{
    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION::" << type <<"::"<< typeid(*this).name() << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }
}

void Scene::checkLinkErrors(GLuint program)
{
    GLint success;
    GLchar infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        std::cout << "ERROR::PROGRAM_LINKING::" << typeid(*this).name() <<"\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }
}


int Scene::loadShaderProgram(const char* vShaderCode, const char* fShaderCode)
{
    unsigned int vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    unsigned int programId = glCreateProgram();
    glAttachShader(programId, vertex);
    glAttachShader(programId, fragment);
    glLinkProgram(programId);
    checkLinkErrors(programId);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return programId;
}