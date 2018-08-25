#include "Image.h"


Image::Image(const DataSet & dataSet, Shader * shaderPtr):
    vao(0),
    vbo(0),
    pointSize(1.0f),
    pointStrength(1.0f),
    shaderPtr(shaderPtr)
{
    for (const Line & line : dataSet.lines) {
        for (size_t j = 0; j < line.GetPointSize(); j ++) {
            const Point & point = line.GetPoint(j);
            vertices.push_back(point.x * 2.0 - 1.0);
            vertices.push_back(point.y * 2.0 - 1.0);
        }
    }
}


Image::~Image()
{
    // release buffer
    glDeleteBuffers(1, &this->vbo);
}


void Image::SetPointSize(const float size)
{
    this->pointSize = size;
}


void Image::SetPointStrength(const float strength)
{
    this->pointStrength = strength;
}


void Image::Render()
{
    this->shaderPtr->Use();

    // bind uniform variables in the shader
    GLint uPointStrengthLoc = glGetUniformLocation(this->shaderPtr->Get(), "uPointStrength");
    glUniform1f(uPointStrengthLoc, this->pointStrength);
    GLint uPointSizeLoc = glGetUniformLocation(this->shaderPtr->Get(), "uPointSize");
    glUniform1f(uPointSizeLoc, this->pointSize);

    // draw points
    glBindVertexArray(this->vao);
    glDrawArrays(GL_POINTS, 0, vertices.size());
    glBindVertexArray(0);
}


void Image::Init()
{
    glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);
    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * this->vertices.size(), &this->vertices.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}


