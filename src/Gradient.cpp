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

/*
    if ( moveDirection.x * (prevPoint.x - point.x) + moveDirection.y * (prevPoint.y - point.y) < 0) {
        moveDirection *= -1;
    }
*/
    double triangleArea = glm::determinant(glm::dmat2(nextPoint - point, prevPoint - point)) / 2.0;
    double moveLength = triangleArea / glm::length(moveDirection);

    dvec2 result = moveLength * glm::normalize(moveDirection);


    auto toLine = [=] (dvec2 p, dvec2 p1, dvec2 p2) mutable throw() -> double {
        return fabs( (p2.x - p1.x) * (p.y - p1.y) - (p2.y - p1.y) * (p.x - p1.x) );
        // x * (a.y - b.y) - y * (a.x - b.x) + a.x * b.y - a.y * b.x = 0
/*
        const double eps = 1e-11;
        const double result = p.x * (p1.y - p2.y) - p.y * (p1.x - p2.x) + p1.x * p2.y - p1.y * p2.x;
        return (result < 0 ? -result : result) < eps;
*/
    };

    if ( toLine(point + result, prevPoint, nextPoint) > toLine(point - result, prevPoint, nextPoint) ) {
        result *= -1;
    }



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
        if (line.GetPointSize() >= 2 && !line.static_) {
            for (size_t j = 0; j < line.GetPointSize(); j++) {
                Point point = line.GetPoint(j);
                point.z = this->GetAccMapValue(point.x, point.y);
                if (!point.fixed && point.waypointId != (size_t)-1) {
                    bool isClosest = false;
//                    size_t closestPointId = -1;

                    const dvec2 gradVec = this->GetGradient(point);
                    const double stepSize = this->steps.GetAdd(stepNum);

                    double scale = 0.0;
                    dvec2 forward = dvec2(0.0, 0.0);

                    //if (!point.isSegment) {
                        if (point.waypointId == (size_t)-1) throw "waypoint id = -1";
                        const Waypoint & waypoint = line.GetWaypointFromPointId(j);
//                        closestPointId = waypoint.closestPointId;
                        if (j == waypoint.closestPointId) {
                            isClosest = true;
                            scale = 1.0;
                            forward = dvec2(waypoint.x - point.x, waypoint.y - point.y);
                            // closest point must reach it's waypoint at last step


                            if (stepNum == this->steps.GetSize() - 1 && point.preparedWaypoint) {
                                point.id = waypoint.id;
                                point.fixed = true;
                                // point.preparedWaypoint = false;
                            }
                        }
                        
                        // if (point.preparedWaypoint && stepNum == this->steps.GetSize() - 1) {
                        //     point.id = waypoint.id;
                        //     point.fixed = true;
                        //     point.preparedWaypoint = false;
                        // }
                    //} else {
                    //   scale = 1.0;
                    //}

					try {
                    if (!isClosest) {
                        const Point & prevPoint = line.GetPoint(point.prevFixedPointId);
                        const Point & nextPoint = line.GetPoint(point.nextFixedPointId);
                        const double d1 = glm::distance( dvec2(prevPoint.x, prevPoint.y), dvec2(point.x, point.y) );
                        const double d2 = glm::distance( dvec2(nextPoint.x, nextPoint.y), dvec2(point.x, point.y) );
                        scale = std::min(d1, d2) / (d1 + d2);
/*
                        if (closestPointId != (size_t)-1 && j > closestPointId) {
                            scale = 1.0 - scale;
                        }
*/
                        dvec2 w1, w2;
                        size_t id1, id2;
                        if (!prevPoint.fixed && prevPoint.waypointId != (size_t)-1) {
                            const Waypoint & prevWaypoint = line.GetWaypoint(prevPoint.waypointId);
                            w1 = dvec2(prevWaypoint.x, prevWaypoint.y);
                            id1 = prevWaypoint.id;
                        } else {
                            w1 = dvec2(prevPoint.x, prevPoint.y);
                            id1 = prevPoint.id;
                        }

                        if (!nextPoint.fixed && nextPoint.waypointId != (size_t)-1) {
                            const Waypoint & nextWaypoint = line.GetWaypoint(nextPoint.waypointId);
                            w2 = dvec2(nextWaypoint.x, nextWaypoint.y);
                            id2 = nextWaypoint.id;
                        } else {
                            w2 = dvec2(nextPoint.x, nextPoint.y);
                            id2 = nextPoint.id;
                        }

                        forward = this->GetForwordDirection(w1, w2, dvec2(point.x, point.y));

                        if ( line.segments.find( std::make_pair( std::min(id1, id2), std::max(id1, id2) ) ) != line.segments.end() ) {
                            scale = 1.0;
                        }
                    }
					} catch (...) {
						scale = 1.0;
                        forward = dvec2(waypoint.x - point.x, waypoint.y - point.y);
					}


					//scale = 1.0;
                    dvec2 newPoint = dvec2(point.x, point.y) - (1.0 - scale) * gradVec + (scale * stepSize) * forward;

                    if (newPoint.x < 0.0 || newPoint.x > 1.0 || newPoint.y < 0.0 || newPoint.y > 1.0) {
                        newPoint = dvec2(point.x, point.y);
                    }

// debug
/*
                    if (newPoint.x < 0.1 || newPoint.y < 0.1) {
                        throw "Gradient.cpp:226---";
                        std::cout << point.x << " " << point.y << std::endl;
                    }
*/
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


