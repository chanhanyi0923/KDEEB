#pragma once

#include <cstddef>

class Waypoint
{
public:
    // id of this point in database
    size_t id;

    size_t closestPointId;
    size_t oId; // origin id
    size_t dId; // destination id
    double x, y, minDist;

    Waypoint(double x, double y, size_t id);
    ~Waypoint();
};

