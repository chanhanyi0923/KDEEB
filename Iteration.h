#pragma once
class Iteration
{
public:
	int count, textureWidth;
	float kernelSize;
	double attractionFactor, smoothingFactor;
	bool doResampling;
	Iteration(
		int count,
		int textureWidth,
		float kernelSize,
		double attractionFactor,
		double smoothingFactor,
		bool doResampling
	);
	virtual ~Iteration();
};

