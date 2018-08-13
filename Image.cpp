#include "Image.h"


Image::Image(const DataSet & dataSet, Shader * shaderPtr):
	VAO(0),
	VBO(0),
	pointSize(1.0f),
	pointStrength(1.0f),
	shaderPtr(shaderPtr)
{
	for (const auto & records : dataSet.data) {
		for (const auto & record : records) {
			const float x = record.x * 2.0 - 1.0;
			const float y = record.y * 2.0 - 1.0;
			vertices.push_back(x);
			vertices.push_back(y);
		}
	}
}


Image::~Image()
{
	// release buffer
	glDeleteBuffers(1, &this->VBO);
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
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, vertices.size());
	glBindVertexArray(0);
}


void Image::Init()
{
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * this->vertices.size(), &this->vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}
