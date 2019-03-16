#pragma once
class Iteration
{
public:
	int count, textureWidth;
	float kernelSize;
	double attractionFactor, smoothingFactor;
	bool doResampling;
  int resampleStep;
	Iteration(
		int count,
		int textureWidth,
		float kernelSize,
		double attractionFactor,
		double smoothingFactor,
		bool doResampling,
    int resampleStep
	);
	virtual ~Iteration();
};

