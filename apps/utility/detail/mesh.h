#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

enum eDataIdx { Position = 0, UV = 1 };

struct GLVertex
{
   GLfloat pos[3];
   GLfloat uv[2];
};

std::pair<GLuint, GLuint> meshCreate(const std::vector<GLVertex>& vertices)
{
    GLuint vao = 0, vbo = 0;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLVertex), vertices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(eDataIdx::Position);
        glEnableVertexAttribArray(eDataIdx::UV);
        glVertexAttribPointer(eDataIdx::Position,   3, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (void*) offsetof(GLVertex, pos));
        glVertexAttribPointer(eDataIdx::UV,         2, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (void*) offsetof(GLVertex, uv));
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return std::make_pair(vao, vbo);
}
