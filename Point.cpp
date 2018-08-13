#include "Point.h"


Point::Point() :
	x(0.0), y(0.0), z(0.0), timeInt(0.0), timeNormelized(0.0)
{
}


Point::Point(double x, double y, double z):
	x(x), y(y), z(z), timeInt(0.0), timeNormelized(0.0)
{
}


Point::Point(double x, double y, double z, double timeNormelized, double timeInt):
	x(x), y(y), z(z), timeInt(timeInt), timeNormelized(timeNormelized)
{
}


Point::~Point()
{
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


