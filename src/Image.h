#pragma once

#include <GLES3/gl32.h>
#include "DataSet.h"
#include "Shader.h"

class Image
{
    Shader * shaderPtr;
    GLuint vao, vbo;
    std::vector<GLfloat> vertices;
    GLfloat pointSize, pointStrength;
public:
    Image(const DataSet & dataSet, Shader * shaderPtr);
    ~Image();
    void SetPointSize(const float size);
    void SetPointStrength(const float strength);
    void Render();
    void Init();
};

