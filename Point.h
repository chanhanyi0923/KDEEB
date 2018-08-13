#pragma once
class Point
{
public:
	double x, y, z, timeNormelized, timeInt;
	Point();
	Point(double x, double y, double z);
	Point(double x, double y, double z, double time, double timeInt);
	~Point();
	bool operator == (const Point & point) const;
	bool operator != (const Point & point) const;
};

