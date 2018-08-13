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
	std::string OpenFile(const char *filename);

public:
	Shader();
	virtual ~Shader();
	void LoadVertexShader(const char *filename);
	void LoadFragmentShader(const char *filename);
	void CreateProgram();
	void Use();
	GLuint Get();
};
