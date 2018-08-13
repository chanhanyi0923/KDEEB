#pragma once

#include <cstddef>
#include <vector>
#include "Point.h"

class Line
{
private:
     std::vector<Point> points;

public:
    Line();
    ~Line();
    size_t GetSize() const;
    Point GetPoint(size_t index) const;
    Point GetFirstPoint() const;
    void SetFirstPoint(const Point & point);
    Point GetLastPoint() const;
    void SetLastPoint(const Point & point);
    void SetPoint(size_t index, const Point & point);
    void AddPoint(const Point & point);
};

