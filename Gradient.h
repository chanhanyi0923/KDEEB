#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "DataSet.h"
#include "Steps.h"

class Gradient
{
private:
    int width;
    double attractionFactor;
    std::vector<float> x;
    std::vector<float> y;
    const std::vector<float> * accMapPtr;

    Steps steps;

    double GetStepSize(const int stepNum) const;
    //double CalculateStepSize(const int totalStepNum, const double orig_minDist, const int step_i);
    size_t GetAccMapIndexNormalized(const double x, const double y) const;
    float GetAccMapValue(const double x, const double y) const;
    void ComputeGradient();

public:
    Gradient();
    ~Gradient();
    void Resize(const size_t size);
    void SetAttractionFactor(const double attractionFactor);
    void SetAccMap(const std::vector<float> * accMapPtr);
    void SetWidth(const int width);
    void ApplyGradient(DataSet & dataSet);
    void ApplyGradientWithWaypoint(DataSet & dataSet, const size_t stepNum);

    void InitSteps(const size_t stepTotalNum);
};

