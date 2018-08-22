#pragma once

#include <cstddef>

class Point
{
public:
    // if the point in a segment
    bool isSegment;

    // if the point is fixed
    bool fixed;

    // id of the waypoint of this point
    size_t waypointId;

	double x, y, z, timeNormelized, timeInt;
	Point();
	Point(double x, double y, double z);
	Point(double x, double y, double z, double time, double timeInt);
	~Point();

    bool Compare2D(const Point & point) const;
	bool operator == (const Point & point) const;
	bool operator != (const Point & point) const;
};

