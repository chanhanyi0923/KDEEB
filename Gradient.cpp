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
            glm::dvec2 dvec2d = glm::dvec2(num3, num4);

            // 5E-06f => eps
            if (glm::length(dvec2d) > 5E-06f) {
                dvec2d = glm::normalize(dvec2d);
                this->x[index] = dvec2d.x * (this->attractionFactor / 100.0f);
                this->y[index] = dvec2d.y * (this->attractionFactor / 100.0f);
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


glm::dvec2 Gradient::GetGradient(const Point & point)
{
    const size_t index = this->GetAccMapIndexNormalized(point.x, point.y);
    return glm::dvec2(this->x[index], this->y[index]);
}


glm::dvec2 Gradient::GetForwordDirection(glm::dvec2 prevPoint, glm::dvec2 nextPoint, glm::dvec2 point)
{
    using glm::dvec2;
    // the direction point move forward
    dvec2 moveDirection = dvec2(prevPoint.y - nextPoint.y, nextPoint.x - prevPoint.x);

    if (glm::dot(moveDirection, prevPoint - point) < 0) {
        moveDirection *= -1;
    }

    double triangleArea = glm::determinant(glm::dmat2(nextPoint - point, prevPoint - point)) / 2.0;
    double moveLength = triangleArea / glm::length(moveDirection);

    dvec2 result = moveLength * glm::normalize(moveDirection);

    return result;
}


void Gradient::ApplyGradientWithWaypoint(DataSet & dataSet, const size_t stepNum)
{
    using glm::dvec2;
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
                    const Waypoint & waypoint = line.GetWaypointFromPointId(j);
                    //const size_t index = this->GetAccMapIndexNormalized(point.x, point.y);

                    dvec2 toWaypoint = glm::normalize( dvec2(waypoint.x - point.x, waypoint.y - point.y) );
                    const dvec2 gradVec = this->GetGradient(point);
                    /*
                    double toWaypointX = waypoint.x - point.x;
                    double toWaypointY = waypoint.y - point.y;
                    const double toWaypointLength = std::sqrt(toWaypointX * toWaypointX + toWaypointY * toWaypointY);
                    toWaypointX /= toWaypointLength;
                    toWaypointY /= toWaypointLength;
                    */
                    const double stepSize = this->steps.GetAdd(stepNum, waypoint.minDist);
                    //double stepSize = this->CalculateStepSize(stepNum, waypoint.minDist, stepI);

                    //double xNew = point.x - this->x[index] + toWaypointX * stepSize;
                    //double yNew = point.y - this->y[index] + toWaypointY * stepSize;
                    //double xNew, yNew;
                    

/*
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
                    } else if (point.isSegment) {
                        xNew = point.x + toWaypointX * stepSize;
                        yNew = point.y + toWaypointY * stepSize;
                    } else {
                        const Point & prevPoint = line.GetPoint(point.prevFixedPointId);
                        const Point & nextPoint = line.GetPoint(point.nextFixedPointId);
                        double d1 = sqrt( (prevPoint.x - point.x) * (prevPoint.x - point.x) + (prevPoint.y - point.y) * (prevPoint.y - point.y) );
                        double d2 = sqrt( (nextPoint.x - point.x) * (nextPoint.x - point.x) + (nextPoint.y - point.y) * (nextPoint.y - point.y) );
                        double f = d1 / (d1 + d2);

                        if (j > waypoint.closestPointId) {
                            f = 1.0 - f;
                        }

                        xNew = point.x - (1.0 - f) * this->x[index] + f * toWaypointX * stepSize;
                        yNew = point.y - (1.0 - f) * this->y[index] + f * toWaypointY * stepSize;
                    }
*/
                    


                    double scale = 0.0;
                    if (j == waypoint.closestPointId || point.isSegment) {
                        scale = 1.0;
                    } else {
                        const Point & prevPoint = line.GetPoint(point.prevFixedPointId);
                        const Point & nextPoint = line.GetPoint(point.nextFixedPointId);
                        const double d1 = glm::distance( dvec2(prevPoint.x, prevPoint.y), dvec2(point.x, point.y) );
                        const double d2 = glm::distance( dvec2(nextPoint.x, nextPoint.y), dvec2(point.x, point.y) );
                        scale = d1 / (d1 + d2);

                        if (j > waypoint.closestPointId) {
                            scale = 1.0 - scale;
                        }

                        dvec2 w1, w2;
                        if (prevPoint.waypointId != (size_t)-1) {
                            const Waypoint & prevWaypoint = line.GetWaypoint(prevPoint.waypointId);
                            w1 = dvec2(prevWaypoint.x, prevWaypoint.y);
                        } else {
                            w1 = dvec2(prevPoint.x, prevPoint.y);
                        }

                        if (nextPoint.waypointId != (size_t)-1) {
                            const Waypoint & nextWaypoint = line.GetWaypoint(nextPoint.waypointId);
                            w2 = dvec2(nextWaypoint.x, nextWaypoint.y);
                        } else {
                            w2 = dvec2(nextPoint.x, nextPoint.y);
                        }

                        toWaypoint = this->GetForwordDirection(w1, w2, dvec2(point.x, point.y));
                    }

                    
                    dvec2 newPoint = dvec2(point.x, point.y) - (1.0 - scale) * gradVec + scale * (toWaypoint * stepSize);


                    // closest point must reach it's waypoint at last step
                    if (j == waypoint.closestPointId && stepNum == this->steps.GetSize() - 1) {
                        newPoint = dvec2(waypoint.x, waypoint.y);
                        point.fixed = true;
                    }


                    if (newPoint.x < 0.0 || newPoint.x > 1.0 || newPoint.y < 0.0 || newPoint.y > 1.0) {
                        newPoint = dvec2(point.x, point.y);
                    }

                    point.x = newPoint.x;
                    point.y = newPoint.y;
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


