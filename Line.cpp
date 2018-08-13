#include "Line.h"

Line::Line()
{
}


Line::~Line()
{
}


size_t Line::GetSize() const
{
    return this->points.size();
}


Point Line::GetPoint(size_t index) const
{
    return this->points[index];
}


Point Line::GetFirstPoint() const
{
    return this->points.front();
}


void Line::SetFirstPoint(const Point & point)
{
    this->points.front() = point;
}


Point Line::GetLastPoint() const
{
    return this->points.back();
}


void Line::SetLastPoint(const Point & point)
{
    this->points.back() = point;
}


void Line::SetPoint(size_t index, const Point & point)
{
    this->points[index] = point;
}


void Line::AddPoint(const Point & point)
{
    this->points.push_back(point);
}


