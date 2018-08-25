#pragma once

#include <cstddef>

class Waypoint
{
public:
    size_t closestPointId;
    size_t oId; // origin id
    size_t dId; // destination id
    double x, y, minDist;

    Waypoint(double x, double y);
    ~Waypoint();
};

