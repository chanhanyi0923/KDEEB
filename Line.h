#pragma once

#include <cstddef>
#include <vector>
#include <map>
#include "Point.h"
#include "Waypoint.h"

class Line
{
private:
    std::vector<Point> points;
    std::vector<Waypoint> waypoints;

    // to deal with multiple waypoints in a interval;
    struct Interval
    {
        std::vector<size_t> waypointIds;
        size_t lowerBound;
        Interval(): lowerBound(0) {}
    };
    std::map< std::pair<size_t, size_t>, Interval > intervals;

public:
    Line();
    ~Line();

    // point
    size_t GetPointSize() const;
    Point GetPoint(const size_t index) const;
    Point GetFirstPoint() const;
    void SetFirstPoint(const Point & point);
    Point GetLastPoint() const;
    void SetLastPoint(const Point & point);
    void SetPoint(const size_t index, const Point & point);
    void AddPoint(const Point & point);
    size_t FindPointIndex(const Point & point) const;

    // waypoint
    void AddSegment(const size_t oId, const size_t dId);
    void AddWaypoint(const size_t oId, const size_t dId, Waypoint waypoint);
    void AddWaypoints(const Line & line);
    void UpdatePoints();
    Waypoint GetWaypointFromPoint(const size_t index) const;
    void SetWaypoint(const size_t index, const Waypoint & waypoint);
    size_t GetWaypointSize() const;
    void ClearWaypoints();
    void RemovePointsInSegment();
};

