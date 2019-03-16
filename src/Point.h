#pragma once

#include <cstddef>
#include <utility>

class Point
{
public:
    // the id of this point in database (for compare if two points are 'equal')
    // don't use eps or == to compare two points
    size_t id;

    // previous fixed point id
    // ( = -1 if the point itself is fixed of is others' closest point)
    size_t prevFixedPointId;

    // next fixed point id
    // ( = -1 if the point itself is fixed of is others' closest point)
    size_t nextFixedPointId;

    // if the point in a segment
    bool isSegment;

    // if the point is fixed
    bool fixed;

    // 
    bool preparedWaypoint;

    // id of the waypoint of this point
    size_t waypointId;

	double x, y, z, timeNormelized, timeInt;
	Point();
	Point(double x, double y, double z);
	Point(double x, double y, double z, double time, double timeInt);
	~Point();

    bool CompareId(const Point & point) const;
	//bool operator == (const Point & point) const;
	//bool operator != (const Point & point) const;
};

