#include "Waypoint.h"

Waypoint::Waypoint(double x, double y, size_t id):
    id(id),
    x(x),
    y(y),
    closestPointId(-1),
    oId(-1),
    dId(-1),
    minDist(0.0)
{
}


Waypoint::~Waypoint()
{
}


