#include "Gradient.h"


Gradient::Gradient():
    width(0),
    accMapPtr(nullptr),
    attractionFactor(0.0)
{
}


Gradient::~Gradient()
{
}


void Gradient::Resize(const size_t size)
{
    this->x.resize(size);
    this->y.resize(size);
}


void Gradient::SetAttractionFactor(const double attractionFactor)
{
    this->attractionFactor = attractionFactor;
}


void Gradient::SetAccMap(const std::vector<float> * accMapPtr)
{
    this->accMapPtr = accMapPtr;
}


void Gradient::SetWidth(const int width)
{
    this->width = width;
}


void Gradient::ComputeGradient()
{
    const std::vector<float> & accMap = *(this->accMapPtr);

    #pragma omp parallel for
    for (int y = 0; y < width - 1; y++) {
        const int curLineOffs = y * width;

        #pragma omp parallel for
        for (int x = 0; x < width - 1; x++) {
            int index = x + curLineOffs;
            float num2 = accMap[index];
            float num3 = num2 - accMap[index + 1];
            float num4 = num2 - accMap[index + width];
            glm::vec2 vec2d = glm::vec2(num3, num4);

            // 5E-06f => eps
            if (glm::length(vec2d) > 5E-06f) {
                vec2d = glm::normalize(vec2d);
                this->x[index] = vec2d.x * (this->attractionFactor / 100.0f);
                this->y[index] = vec2d.y * (this->attractionFactor / 100.0f);
            }
        }
    }
}


void Gradient::ApplyGradient(DataSet & dataSet, const double obstacleRadius)
{
    this->ComputeGradient();

    #pragma omp parallel for
    for (size_t i = 0; i < dataSet.lines.size(); i ++) {
        Line & line = dataSet.lines[i];
        if (line.GetSize() >= 2) {
            Point firstPoint = line.GetFirstPoint();
            firstPoint.z = this->GetAccMapValue(firstPoint.x, firstPoint.y);
            line.SetFirstPoint(firstPoint);

            for (size_t j = 1; j < line.GetSize() - 1; j++) {
                Point point = line.GetPoint(j);
                const size_t index = this->GetAccMapIndexNormalized(point.x, point.y);
                double xNew = point.x - this->x[index];
                double yNew = point.y - this->y[index];
                if (xNew < 0.0 || xNew > 1.0 || yNew < 0.0 || yNew > 1.0) {
                    xNew = point.x;
                    yNew = point.y;
                }
                point.x = xNew;
                point.y = yNew;
                point.z = (*(this->accMapPtr))[index] + 0.02f;
                line.SetPoint(j, point);
            }

            Point lastPoint = line.GetLastPoint();
            lastPoint.z = this->GetAccMapValue(lastPoint.x, lastPoint.y);
            line.SetLastPoint(lastPoint);
        }
    }
}


float Gradient::GetAccMapValue(const double x, const double y) const
{
    const size_t xIndex = x * (this->width - 1);
    const size_t yIndex = y * (this->width - 1);
    const size_t index = xIndex + yIndex * this->width;
    return (*(this->accMapPtr))[index];
}


size_t Gradient::GetAccMapIndexNormalized(double x, double y) const
{
    const size_t xIndex = x * (this->width - 1);
    const size_t yIndex = y * (this->width - 1);
    return xIndex + yIndex * this->width;
}


