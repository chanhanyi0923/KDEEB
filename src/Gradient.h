#pragma once

#include <vector>
#include "DataSet.h"
#include "Steps.h"
#include <glm/glm.hpp>

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
    glm::dvec2 GetGradient(const Point & point);
    glm::dvec2 GetForwordDirection(glm::dvec2 prevPoint, glm::dvec2 nextPoint, glm::dvec2 point);

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

