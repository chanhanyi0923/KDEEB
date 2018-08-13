#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Record.h"
#include "DataSet.h"

class Gradient
{
public:
	std::vector<float> gradientX;
	std::vector<float> gradientY;

	Gradient();
	virtual ~Gradient();
	void Resize(size_t size);
	void ComputeGradient(const vector<float>& accMap, int gradientW, float attractionFactor);
	void ApplyGradient(DataSet & dataSet, const vector<float>& accMap, int gradientW, double attractionFactor, double obstacleRadius);
	int GetAccMapIndexNormalized(double x, double y, int width);
};

