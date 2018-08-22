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


void Gradient::ApplyGradient(DataSet & dataSet)
{
    this->ComputeGradient();

    #pragma omp parallel for
    for (size_t i = 0; i < dataSet.lines.size(); i ++) {
        Line & line = dataSet.lines[i];
        if (line.GetPointSize() >= 2) {
            Point firstPoint = line.GetFirstPoint();
            firstPoint.z = this->GetAccMapValue(firstPoint.x, firstPoint.y);
            line.SetFirstPoint(firstPoint);

            for (size_t j = 1; j < line.GetPointSize() - 1; j++) {
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


void Gradient::ApplyGradientWithWaypoint(DataSet & dataSet, const size_t stepNum)
{
    //const double obstacleRadius

    this->ComputeGradient();

    #pragma omp parallel for
    for (size_t i = 0; i < dataSet.lines.size(); i ++) {
        Line & line = dataSet.lines[i];
        if (line.GetPointSize() >= 2) {
            for (size_t j = 0; j < line.GetPointSize(); j++) {
                Point point = line.GetPoint(j);
                point.z = this->GetAccMapValue(point.x, point.y);
                if (!point.fixed) {
                    const Waypoint & waypoint = line.GetWaypointFromPoint(j);
                    const size_t index = this->GetAccMapIndexNormalized(point.x, point.y);
                    double toWaypointX = waypoint.x - point.x;
                    double toWaypointY = waypoint.y - point.y;
                    const double toWaypointLength = std::sqrt(toWaypointX * toWaypointX + toWaypointY * toWaypointY);
                    toWaypointX /= toWaypointLength;
                    toWaypointY /= toWaypointLength;

                    double stepSize = this->steps.GetAdd(stepNum, waypoint.minDist);
                    //double stepSize = this->CalculateStepSize(stepNum, waypoint.minDist, stepI);

                    double xNew = point.x - this->x[index] + toWaypointX * stepSize;
                    double yNew = point.y - this->y[index] + toWaypointY * stepSize;

                    // if this point is closest point
                    if (j == waypoint.closestPointId) {
                        if (stepNum == this->steps.GetSize() - 1) {
                            // maybe need to be changed ?
                            xNew = waypoint.x;
                            yNew = waypoint.y;
                            point.fixed = true;
                        } else {
                            xNew = point.x + toWaypointX * stepSize;
                            yNew = point.y + toWaypointY * stepSize;
                        }
                    }

                    if (point.isSegment) {
                        xNew = point.x + toWaypointX * stepSize;
                        yNew = point.y + toWaypointY * stepSize;
                    }

                    if (xNew < 0.0 || xNew > 1.0 || yNew < 0.0 || yNew > 1.0) {
                        xNew = point.x;
                        yNew = point.y;
                    }

                    point.x = xNew;
                    point.y = yNew;
                    point.z += 0.02f;
                }
                line.SetPoint(j, point);
            }
        }
    }
}


void Gradient::InitSteps(const size_t stepTotalNum)
{
    this->steps = Steps(stepTotalNum);
}


