#include "demo_scenes/camera_test.h"
#include "glad/glad.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

const char* CameraTest::vShaderCode = R""""(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aEdgeCoord;

out vec2 fEdgeCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    fEdgeCoord = aEdgeCoord;
}
)"""";

const char* CameraTest::fShaderCode = R""""(
#version 330 core

in vec2 fEdgeCoord;

out vec4 outColor;

uniform vec3 edge_color;
uniform vec3 interior_color;
uniform float edge_offset;

void main()
{
    bool edgeBif = fEdgeCoord.x < edge_offset || fEdgeCoord.y < edge_offset;  
    outColor = edgeBif ? vec4(edge_color, 1.0f) : vec4(interior_color, 1.0f);
}
)"""";

const float CameraTest::vertices[120] = {
    -0.5f, -0.5f, -0.5f,    1.0f, 0.0f,   //pos & edge interpolation
     0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,    0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,    0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,    0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,

     0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,    0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,    0.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,    0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,

    -0.5f,  0.5f, -0.5f,    1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,    0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,    0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,    0.0f, 0.0f
};

const unsigned int CameraTest::indices[36] = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4,
    8, 9, 10, 10, 11, 8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20
};

const float CameraTest::CUBE_COLORS[3*5] = { 
    0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
};
const float CameraTest::EDGE_COLOR[3] = { 0.2f, 0.2f, 0.2f };
const float CameraTest::EDGE_OFFSET = 0.05f;
const vec3 CameraTest::TOP_BG_COLOR = vec3( 177.0f/255.0f , 212.0f/255.0f, 224.0f/255.0f );
const vec3 CameraTest::BOTTOM_BG_COLOR = vec3( 6.0f/255.0f, 22.5f/255.0f, 36.0f/255.0f );

const vec3 CameraTest::cubePositions[5] = {
    vec3(0,0,0), vec3(1.5,1.5,1.5), vec3(-1.5,1.5,-1.5), vec3(-1.5,1.5,1.5), vec3(1.5,1.5,-1.5)
};

const vec3 CameraTest::cubeAxis[5]{
    vec3(0,1,0), vec3(-1,0,1), vec3(1,0,-1), vec3(-1, 0, -1), vec3(1, 0, 1)
};

CameraTest::CameraTest()
{
    VBO = 0; VAO = 0; EBO = 0;
    programId = 0;
    rotation = 0;
}

void CameraTest::loadModelData()
{
    //CUBE
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void CameraTest::Start()
{
    programId = loadShaderProgram(vShaderCode, fShaderCode);
    loadModelData();
    glEnable(GL_DEPTH_TEST);
    camera.position = vec3(0, 0, 2);
    camera.up = vec3(0, 1, 0);
    camera.lookAt = vec3(0, 0, 0);
    camera.viewSize = 2;
    camera.nearClip = 0.2f;
    camera.farClip = 10.0f;
    camera.projType = Camera::PERSP;
    background.camera = &camera;
    background.topColor = TOP_BG_COLOR;
    background.bottomColor = BOTTOM_BG_COLOR;
    background.Start();
    cont.camera = &camera;
    cont.moveSpeed = 2.0f;
    cont.Start();
}

void CameraTest::Update(float deltaTime)
{
    cont.Update(deltaTime);
    rotation += 45.0f * deltaTime;
    if (rotation >= 360.0f) rotation -= 360.0f;
}

void CameraTest::Render()
{
    glDisable(GL_DEPTH_TEST);
    background.Render();
    glEnable(GL_DEPTH_TEST);

    glUseProgram(programId);
    mat4 view = camera.GetViewMatrix();
    mat4 projection = camera.GetProjMatrix();
    glUniformMatrix4fv(glGetUniformLocation(programId, "view"), 1, GL_FALSE, value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(programId, "projection"), 1, GL_FALSE, value_ptr(projection));
    glUniform3fv(glGetUniformLocation(programId, "edge_color"), 1, &EDGE_COLOR[0]);
    glUniform3fv(glGetUniformLocation(programId, "interior_color"), 1, &CUBE_COLORS[0]);
    glUniform1f(glGetUniformLocation(programId, "edge_offset"), EDGE_OFFSET);

    glBindVertexArray(VAO);
    for (int i = 0; i < 5; i++) {
        mat4 model = glm::translate(mat4(1.0f), cubePositions[i]);
        model = glm::rotate(model, glm::radians(rotation), cubeAxis[i]);
        glUniformMatrix4fv(glGetUniformLocation(programId, "model"), 1, GL_FALSE, value_ptr(model));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
}

void CameraTest::End()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(programId);
}