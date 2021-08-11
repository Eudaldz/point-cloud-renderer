#include "background.h"
#include "glad/glad.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

using namespace glm;

const char* Background::vShaderCode = R""""(
#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 fColor;

uniform vec3 topColor;
uniform vec3 bottomColor;

uniform vec3 camPosition;
uniform mat4 invProj;
uniform mat4 invView;

void main()
{
    gl_Position = vec4(aPos, 1.0f);
    vec4 pv = invView * invProj * vec4(aPos, 1.0f);
    vec3 v = pv.xyz / pv.w;
    v = normalize(v - camPosition);
    float a = dot(vec3(0,1,0), v);
    a = (a+1)/2.0f;
    fColor = mix(bottomColor, topColor, a);
}
)"""";

const char* Background::fShaderCode = R""""(
#version 330 core

in vec3 fColor;

out vec4 outColor;

void main()
{
    outColor = vec4(fColor, 1.0f);
}
)"""";

const float Background::vertices[12] = {
    -1.0f, -1.0f, 0,
    1.0f, -1.0f, 0,
    1.0f, 1.0f, 0,
    -1.0f, 1.0, 0
};

const unsigned int Background::indices[6] = {
    0, 1, 2, 2, 3, 0
};

Background::Background()
{
    VBO = 0; VAO = 0; EBO = 0;
    programId = 0;
    camera = nullptr;
    topColor = vec3(0.8f, 0.8f, 1);
    bottomColor = vec3(0.15f, 0.15f, 0.15f);
}

void Background::loadModelData()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Background::Start()
{
    programId = loadShaderProgram(vShaderCode, fShaderCode);
    loadModelData();
}

void Background::Update(float deltaTime)
{}

void Background::Render()
{
    if (camera == nullptr)return;
    glUseProgram(programId);
    mat4 invView = glm::inverse(camera->GetViewMatrix());
    mat4 invProj = glm::inverse(camera->GetProjMatrix());
    glUniformMatrix4fv(glGetUniformLocation(programId, "invView"), 1, GL_FALSE, value_ptr(invView));
    glUniformMatrix4fv(glGetUniformLocation(programId, "invProj"), 1, GL_FALSE, value_ptr(invProj));
    glUniform3fv(glGetUniformLocation(programId, "topColor"), 1, value_ptr(topColor));
    glUniform3fv(glGetUniformLocation(programId, "bottomColor"), 1, value_ptr(bottomColor));
    glUniform3fv(glGetUniformLocation(programId, "camPosition"), 1, value_ptr(camera->position));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Background::End()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(programId);
}