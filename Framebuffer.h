#pragma once

#include <GL/gl.h>
#include <GLES3/gl32.h>
#include <cmath>
#include <algorithm>
#include <stdint.h>
#include <vector>
#include "RenderException.h"
#include "DataSet.h"
#include "Image.h"

class Framebuffer
{
	const float pointStrength = 0.3f;
	uint32_t fbo, rbo, textureId, textureWidth;
public:
	Framebuffer(int textureWidth);
	virtual ~Framebuffer();

	void Init();

	std::vector<float> ComputeSplatting(float kernelSize, Image & image) const;
};

