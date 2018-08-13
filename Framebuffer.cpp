#include "Framebuffer.h"


Framebuffer::Framebuffer(int textureWidth) :
	textureId(0),
	fbo(0),
	rbo(0),
	textureWidth(textureWidth)
{
}


Framebuffer::~Framebuffer()
{
	// release buffers
	glDeleteFramebuffers(1, &this->fbo);
	glDeleteRenderbuffers(1, &this->rbo);
}


void Framebuffer::Init()
{
	// initialize renderbuffer
	glGenRenderbuffers(1, &this->rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_R32F, this->textureWidth, this->textureWidth);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// initialize framebuffer
	glGenFramebuffers(1, &this->fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->rbo);

	// check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw RenderException("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	}
}


vector<float> Framebuffer::ComputeSplatting(float kernelSize, const DataSet & dataSet, Image & image) const
{
	image.SetPointSize(kernelSize);
	image.SetPointStrength(this->pointStrength);

	glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);

	// specify buffer to be drawn
	const GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);

	glViewport(0, 0, this->textureWidth, this->textureWidth);

	// depth testing is not used.
	glDisable(GL_DEPTH_TEST);

	// blend
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_POINT_SPRITE);
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);

	float zero = 0.0f;
	glClearBufferfv(GL_COLOR, this->rbo, &zero);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	image.Render();

	vector<float> accF;
	accF.resize(this->textureWidth * this->textureWidth, 0.0f);

	// read pixels from framebuffer
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(0, 0, this->textureWidth, this->textureWidth, GL_RED, GL_FLOAT, &accF.front());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	const float eps = 1e-6;

	const float maxAccu = *std::max_element(accF.begin(), accF.end());
	if (fabs(maxAccu) > eps) {
		#pragma omp parallel for
		for (int i = 0; i < accF.size(); i++) {
			accF[i] /= maxAccu;
		}
	}

	return accF;
}
