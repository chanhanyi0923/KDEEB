#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "DataSet.h"

class Gradient
{
    int width;
    std::vector<float> x;
    std::vector<float> y;
    const std::vector<float> * accMapPtr;
    double attractionFactor;

public:
    Gradient();
    ~Gradient();
    void Resize(const size_t size);
    void SetAttractionFactor(const double attractionFactor);
    void SetAccMap(const std::vector<float> * accMapPtr);
    void SetWidth(const int width);
    void ComputeGradient();
    void ApplyGradient(DataSet & dataSet, const double obstacleRadius);

    size_t GetAccMapIndexNormalized(const double x, const double y) const;
    float GetAccMapValue(const double x, const double y) const;
};

