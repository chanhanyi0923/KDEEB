#include "Iteration.h"


Iteration::Iteration(
	int count,
	int textureWidth,
	float kernelSize,
	double attractionFactor,
	double smoothingFactor,
	bool doResampling,
	int resampleStep
):
	count(count),
	textureWidth(textureWidth),
	kernelSize(kernelSize),
	attractionFactor(attractionFactor),
	smoothingFactor(smoothingFactor),
	doResampling(doResampling),
	resampleStep(resampleStep)
{
}


Iteration::~Iteration()
{
}

