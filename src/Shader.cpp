#include "Shader.h"


Shader::Shader():
	program(0),
	vertex_shader(0),
	fragment_shader(0),
	vs(false),
	fs(false)
{
}


Shader::~Shader()
{
}


void Shader::LoadVertexShader()
{
    const GLchar *shader_code =
    #include "vertex.glsl"
    ;

	GLint success;

	// Vertex Shader
	this->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(this->vertex_shader, 1, &shader_code, NULL);
	glCompileShader(this->vertex_shader);
	glGetShaderiv(this->vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar log[512];
		glGetShaderInfoLog(this->vertex_shader, 512, NULL, log);

		std::stringstream buffer;
		buffer << "Vertex shader compilation failed: " << log << std::endl;
		throw RenderException(buffer.str().c_str());
	}
	this->vs = true;
}


void Shader::LoadFragmentShader()
{
	const GLchar *shader_code =
    #include "fragment.glsl"
    ;

	GLint success;

	// Fragment Shader
	this->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(this->fragment_shader, 1, &shader_code, NULL);
	glCompileShader(this->fragment_shader);
	glGetShaderiv(this->fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar log[512];
		glGetShaderInfoLog(this->fragment_shader, 512, NULL, log);

		std::stringstream buffer;
		buffer << "Fragment shader compilation failed: " << log << std::endl;
		throw RenderException(buffer.str().c_str());
	}
	this->fs = true;
}


void Shader::CreateProgram()
{
	GLint success;

	this->program = glCreateProgram();
	if (this->vs) {
		glAttachShader(this->program, this->vertex_shader);
	}
	if (this->fs) {
		glAttachShader(this->program, this->fragment_shader);
	}

	glLinkProgram(this->program);
	glGetProgramiv(this->program, GL_LINK_STATUS, &success);
	if (!success) {
		GLchar log[512];
		glGetProgramInfoLog(this->program, 512, NULL, log);

		std::stringstream buffer;
		buffer << "Shader program linking failed: " << log << std::endl;
		throw RenderException(buffer.str().c_str());
	}

	if (this->vs) {
		glDeleteShader(this->vertex_shader);
	}
	if (this->fs) {
		glDeleteShader(this->fragment_shader);
	}
}


void Shader::Use()
{
	glUseProgram(this->program);
}


GLuint Shader::Get()
{
	return this->program;
}

