#pragma once

#include <GLES3/gl32.h>
#include <string>
#include <fstream>
#include <sstream>
#include "RenderException.h"

class Shader
{
private:
	GLuint program, vertex_shader, fragment_shader;
	bool vs, fs;

public:
	Shader();
	~Shader();
	void LoadVertexShader();
	void LoadFragmentShader();
	void CreateProgram();
	void Use();
	GLuint Get();
};
