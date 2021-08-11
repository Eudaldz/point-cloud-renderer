#include "demo_scenes/cube_test.h"
#include "glad/glad.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

const char* CubeTest::vShaderCode = R""""(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aEdgeCoord;

out vec3 fColor;
out vec2 fEdgeCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    fColor = aColor;
    fEdgeCoord = aEdgeCoord;
}
)"""";

const char* CubeTest::fShaderCode = R""""(
#version 330 core

in vec3 fColor;
in vec2 fEdgeCoord;

out vec4 outColor;

uniform vec3 edge_color;
uniform float edge_offset;

void main()
{
    bool edgeBif = fEdgeCoord.x < edge_offset || fEdgeCoord.y < edge_offset;  
    outColor = edgeBif ? vec4(edge_color, 1.0f) : vec4(fColor, 1.0f);
    //outColor = vec4(fColor, 1.0f);
}
)"""";

const float CubeTest::vertices[192] = {
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,   //pos & color & edge interpolation
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,

    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 0.0f
};

const unsigned int CubeTest::indices[36] = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4,
    8, 9, 10, 10, 11, 8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20
};

const float CubeTest::CAM_ROT_SPEED = 45;
const float CubeTest::EDGE_COLOR[3] = { 0.2f, 0.2f, 0.2f };
const float CubeTest::EDGE_OFFSET = 0.05f;
const float CubeTest::BACKGROUND_COLOR[3] = { 134.0f / 255.0f, 139.0f / 255.0f, 142.0f / 255.0f };

CubeTest::CubeTest()
{
    VBO = 0; VAO = 0; EBO = 0;
    programId = 0;
}

void CubeTest::loadModelData()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void CubeTest::Start()
{
    programId = loadShaderProgram(vShaderCode, fShaderCode);
    loadModelData();
    glEnable(GL_DEPTH_TEST);
    camera.position = vec3(2, 0, 0);
    camera.up = vec3(0, 1, 0);
    camera.lookAt = vec3(0, 0, 0);
    camera.viewSize = 2;
    camera.nearClip = 0.5f;
    camera.farClip = 5.0f;
    camera.projType = Camera::PERSP;
}

void CubeTest::Update(float deltaTime)
{
    mat4 t = mat4(1.0f);
    t = glm::rotate(t, glm::radians(CAM_ROT_SPEED * deltaTime), vec3(0, 1, 1));
    camera.position = t * vec4(camera.position, 1.0f);
}

void CubeTest::Render()
{
    glClearColor(BACKGROUND_COLOR[0], BACKGROUND_COLOR[1], BACKGROUND_COLOR[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(programId);
    mat4 view = camera.GetViewMatrix();
    mat4 projection = camera.GetProjMatrix();
    mat4 model = mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(programId, "model"), 1, GL_FALSE, value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(programId, "view"), 1, GL_FALSE, value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(programId, "projection"), 1, GL_FALSE, value_ptr(projection));
    glUniform3fv(glGetUniformLocation(programId, "edge_color"), 1, &EDGE_COLOR[0]);
    glUniform1f(glGetUniformLocation(programId, "edge_offset"), EDGE_OFFSET);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void CubeTest::End()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(programId);
}