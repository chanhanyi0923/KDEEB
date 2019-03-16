#include "Point.h"

#include <limits>
#include <cmath>


Point::Point() :
    id(-1),
	x(0.0), y(0.0), z(0.0), timeInt(0.0), timeNormelized(0.0),
    waypointId(-1), fixed(false), isSegment(false),
    prevFixedPointId(-1),
    nextFixedPointId(-1),
    preparedWaypoint(false)
{
}


Point::Point(double x, double y, double z):
    id(-1),
	x(x), y(y), z(z), timeInt(0.0), timeNormelized(0.0),
    waypointId(-1), fixed(false), isSegment(false),
    prevFixedPointId(-1),
    nextFixedPointId(-1),
    preparedWaypoint(false)
{
}


Point::Point(double x, double y, double z, double timeNormelized, double timeInt):
    id(-1),
	x(x), y(y), z(z), timeInt(timeInt), timeNormelized(timeNormelized),
    waypointId(-1), fixed(false), isSegment(false),
    prevFixedPointId(-1),
    nextFixedPointId(-1),
    preparedWaypoint(false)
{
}


Point::~Point()
{
}


bool Point::CompareId(const Point & point) const
{
    if (this->id == (size_t)-1 || point.id == (size_t)-1) {
        return false;
    }
    return this->id == point.id;
}

/*
bool Point::Compare2D(const Point & point) const
{
    const double eps = 1e-10;
    const double dx = this->x - point.x;
    const double dy = this->y - point.y;
    return dx * dx + dy * dy < eps;
}


bool Point::operator == (const Point & p) const
{
	return this->x == p.x &&
		this->y == p.y &&
		this->z == p.z &&
		this->timeInt == p.timeInt &&
		this->timeNormelized == p.timeNormelized;
}


bool Point::operator != (const Point & point) const
{
	return !(*this == point);
}
*/


