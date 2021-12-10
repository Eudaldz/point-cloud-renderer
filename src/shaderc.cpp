#include "shaderc.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace {
    void checkCompileErrors(GLuint shader, std::string shaderName)
    {
        GLint success;
        GLchar infoLog[1024];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION::" << shaderName << "::" << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    
    void checkLinkErrors(GLuint program, std::string shaderName)
    {
        GLint success;
        GLchar infoLog[1024];
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(program, 1024, nullptr, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING::" << shaderName << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

GLuint ShaderC::LoadShaderProgram(std::string vertexRes, std::string fragmentRes) 
{
    std::ifstream vertfs("shaders/" + vertexRes);
    std::ifstream fragfs("shaders/" + fragmentRes);
    std::stringstream verts;
    std::stringstream frags;
    verts << vertfs.rdbuf();
    frags << fragfs.rdbuf();

    std::string vertexCode = verts.str();
    std::string fragmentCode = frags.str();

    const char* vCode = vertexCode.c_str();
    const char* fCode = fragmentCode.c_str();
    
    GLuint vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, vertexRes);

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fCode, nullptr);
    glCompileShader(fragment);
    checkCompileErrors(fragment, fragmentRes);

    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertex);
    glAttachShader(programId, fragment);
    glLinkProgram(programId);
    checkLinkErrors(programId, vertexRes + " + " + fragmentRes);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return programId;
}