#include "Line.h"

// debug
#include <iostream>
//
#include <limits>
#include <cmath>

Line::Line()
{
}


Line::~Line()
{
}


//debug
bool Line::checkPoints()
{
    for (size_t i = 0; i < this->points.size(); i ++) {
        const Point & point = this->points[i];
        if (point.x < 0.1 || point.y < 0.1) {
            std::cout << i << " " << point.x << " " << point.y << std::endl;
            return false;
        }
    }
    return true;
}


size_t Line::GetPointSize() const
{
    return this->points.size();
}


Point Line::GetPoint(const size_t index) const
{
    if (index >= this->points.size()) {
        throw "out of index";
    }
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


void Line::SetPoint(const size_t index, const Point & point)
{
    this->points[index] = point;
}


void Line::AddPoint(const Point & point)
{
/*
    if (point.x < 0.1 || point.y < 0.1) {
        std::cout << "!!!" << std::endl;
    }
*/
    this->points.push_back(point);
}


Waypoint Line::GetWaypointFromPointId(const size_t index) const
{
    return this->waypoints[this->points[index].waypointId];
}


Waypoint Line::GetWaypoint(const size_t index) const
{
    return this->waypoints[index];
}


void Line::SetWaypoint(const size_t index, const Waypoint & waypoint)
{
    this->waypoints[index] = waypoint;
}


void Line::ClearWaypoints()
{
    this->waypoints.clear();
    this->intervals.clear();
    this->segments.clear();
}


size_t Line::FindPointIndexById(const size_t id) const
{
    size_t index = -1;
    for (size_t i = 0; i < this->GetPointSize(); i ++) {
        const Point & point = this->GetPoint(i);
        if (id != (size_t)-1 && point.id != (size_t)-1 && point.id == id) {
            index = i;
            break;
        }
    }

    if (index == (size_t)-1) {

/*
        //int fixedNum = 0;
        for (size_t i = 0; i < this->GetPointSize(); i ++) {
            if (this->GetPoint(i).fixed) {
                std::cout << this->GetPoint(i).id << std::endl;
//                fixedNum ++;
            }
        }
        std::cout << std::endl << id << std::endl;
        std::cout << "line id: " << this->id << std::endl;
        //std::cout << fixedNum << std::endl;
*/
        std::cout << "line id: " << this->id << std::endl;

        throw "index = -1";
    }

    return index;
}


/*
void Line::AddSegment(const size_t oId, const size_t dId)
{
    if (dId < oId) {
        throw "!!!";
//        std::swap(oId, dId);
    }

    if (!this->points[oId].fixed) {
        throw "!!!";
    }

    if (!this->points[dId].fixed) {
        throw "!!!";
    }

    for (size_t i = oId; i <= dId; i ++) {
        this->points[i].isSegment = true;
    }
}
*/

// these are id of point in database, not id in this line
void Line::AddSegment(const size_t oId, const size_t dId)
{
    using std::pair;

    pair<size_t, size_t> pii = std::make_pair(oId, dId);
    if (pii.first > pii.second) {
        std::swap(pii.first, pii.second);
    }

    this->segments.insert(pii);
}


size_t Line::GetWaypointSize() const
{
    return this->waypoints.size();
}


void Line::AddWaypoint(const size_t oId, const size_t dId, Waypoint waypoint)
{
    if (dId < oId) {
        throw "!!!";
//        std::swap(oId, dId);
    }

    waypoint.oId = oId;
    waypoint.dId = dId;

    Interval & interval = this->intervals[ std::make_pair(waypoint.oId, waypoint.dId) ];

    if (this->GetPointSize() == 2) {
        // id = null
        waypoint.closestPointId = std::numeric_limits<size_t>::max();

        const double dx = (this->GetFirstPoint().x + this->GetLastPoint().x) / 2.0 - waypoint.x;
        const double dy = (this->GetFirstPoint().y + this->GetLastPoint().y) / 2.0 - waypoint.x;
        waypoint.minDist = std::sqrt(dx * dx + dy * dy);
    } else if (this->GetPointSize() > 2) {
        double minDist2 = std::numeric_limits<double>::infinity();

        const size_t beginI = std::max(waypoint.oId, interval.lowerBound);
        const size_t endI = std::min(waypoint.dId, this->GetPointSize());
        for (size_t i = beginI; i <= endI; i ++) {
            const Point & point = this->GetPoint(i);

            // skip fixed points
            if (point.fixed) {
                continue;
            }

            double dist2 = (point.x - waypoint.x) * (point.x - waypoint.x) +
                           (point.y - waypoint.y) * (point.y - waypoint.y);
            if (dist2 < minDist2) {
                minDist2 = dist2;
                waypoint.closestPointId = i;
            }
        }
        waypoint.minDist = std::sqrt(minDist2);
        interval.lowerBound = waypoint.closestPointId + 1;
    }

    this->waypoints.push_back(waypoint);
    interval.waypointIds.push_back(this->waypoints.size() - 1);
}


void Line::AddWaypoints(const Line & line)
{
    this->waypoints.clear();
    for (const auto & waypoint: line.waypoints) {
        // do not use waypoint.oId, waypoint.dId, since they are not updated
        const Point & o = line.GetPoint(waypoint.oId);
        const Point & d = line.GetPoint(waypoint.dId);

        // this is the real index in current line
        const size_t oId = this->FindPointIndexById(o.id);
        const size_t dId = this->FindPointIndexById(d.id);
        this->AddWaypoint(oId, dId, waypoint);
    }
}


void Line::UpdatePoints()
{
    for (size_t i = 0; i < this->points.size(); i ++) {
        Point & point = this->points[i];
        bool hasWaypoint = false;
        if (!point.fixed) {
            auto intervalPtr = this->intervals.end();

            // can be acclerate begin
            for (auto it = this->intervals.begin(); it != this->intervals.end(); it ++) {
                const size_t oId = it->first.first;
                const size_t dId = it->first.second;
                if (oId < i && i < dId) {
                    hasWaypoint = true;
                    intervalPtr = it;
                    break;
                } else if (oId == i || dId == i) {
                    if (!point.fixed) throw "Line:245";
                    //point.fixed = true;
                    break;
                }
            }
            // end

            if (hasWaypoint) {
                const auto & ids = intervalPtr->second.waypointIds;
                size_t closestWaypointId = -1;
                int minIndexDist = std::numeric_limits<int>::max();

                std::vector<size_t> fixedPointIds;
                fixedPointIds.push_back(intervalPtr->first.first);

                for (size_t id : ids) {
                    const Waypoint & waypoint = this->waypoints[id];
                    if (waypoint.closestPointId == (size_t)-1) {
                        continue;
                    }

                    fixedPointIds.push_back(waypoint.closestPointId);

                    // distance(index) between this waypoint and point[i]
                    const int indexDist = std::abs((int)waypoint.closestPointId - (int)i);

                    if (indexDist < minIndexDist) {
                        minIndexDist = indexDist;
                        closestWaypointId = id;
                    }
                }

                fixedPointIds.push_back(intervalPtr->first.second);

                if (closestWaypointId == (size_t)-1) {
                    throw "could not find closest waypoint";
                }
                point.waypointId = closestWaypointId;

                const auto closestFixedPointPtr = std::find(fixedPointIds.begin(), fixedPointIds.end(), this->waypoints[closestWaypointId].closestPointId);
                //const auto closestFixedPointPtr = fixedPointIds.find(this->waypoints[closestWaypointId].closestPointId);
                if (i < *closestFixedPointPtr) {
                    point.prevFixedPointId = *std::prev(closestFixedPointPtr);
                    point.nextFixedPointId = *closestFixedPointPtr;
                } else if (i > *closestFixedPointPtr) {
                    point.prevFixedPointId = *closestFixedPointPtr;
                    point.nextFixedPointId = *std::next(closestFixedPointPtr);
                }
            } else {
                //if (!point.isSegment) throw "Line.cpp:293; error";


                int l = std::numeric_limits<int>::min();
                int r = std::numeric_limits<int>::max();

                for (auto it = this->intervals.begin(); it != this->intervals.end(); it ++) {
                    const size_t oId = it->first.first;
                    const size_t dId = it->first.second;
                    if (dId < i) {
                        l = std::max(l, (int)dId);
                    }
                    if (oId > i) {
                        r = std::min(r, (int)oId);
                    }
                }

                if (l == std::numeric_limits<int>::min()) {
                    l = 0;
                }
                if (r == std::numeric_limits<int>::max()) {
                    r = this->points.size();
                }

                point.prevFixedPointId = l;
                point.nextFixedPointId = r;
            }
        }
        if (!hasWaypoint) {
            if (!point.fixed/* && !point.isSegment*/) {


                

                std::cout << "error" << std::endl;
                throw "! error";
            }
            point.waypointId = -1;
        }
    }
/*
    size_t left = -1, right = 0;
    for (size_t i = 0; i < this->points.size(); i ++) {
        if (this->points[i].waypointId == (unsigned long int)-1) {
            left = std::min(left, i);
            right = std::max(right, i);
        } else {
            if (left <= right) {
                //left = fixed, right = fixed
                const double xCenter = (this->points[left].x + this->points[right].x) / 2.0;
                const double yCenter = (this->points[left].y + this->points[right].y) / 2.0;
                Waypoint waypoint(xCenter, yCenter);
                this->AddWaypoint(this->points[left], this->points[right], waypoint);
                const size_t waypointId = this->waypoints.size() - 1;
                for (size_t j = left; j < right; j ++) {
                    this->points[j].waypointId = waypointId;
                }
            }
        }
    }
*/
//
}


void Line::RemovePointsInSegment()
{
    std::vector<Point> newPoints;
/*
    for (size_t i = 0; i < this->points.size(); i ++) {
        newPoints.push_back(this->points[i]);
        if (this->points[i].isSegment) {
            for (i ++; !this->points[i].isSegment && i < this->points.size(); i ++);
            newPoints.push_back(this->points[i]);
        }
    }
*/

/*
    for (size_t i = 0; i < this->points.size(); i ++) {
        const Point & point = this->points[i];
        if (!point.isSegment || point.fixed) {
            newPoints.push_back(point);
        }
    }
*/
    std::vector<bool> keep;
    keep.resize(this->points.size(), true);

    for (const auto & pii : this->segments) {
        size_t oId = this->FindPointIndexById(pii.first);
        size_t dId = this->FindPointIndexById(pii.second);
        if (oId > dId) {
            std::swap(oId, dId);
        }
        this->points[oId].isSegment = true;
        this->points[dId].isSegment = true;
        for (size_t i = oId + 1; i <= dId - 1; i ++) {
            keep[i] = false;
        }
    }

    for (size_t i = 0; i < this->points.size(); i ++) {
        if (keep[i]) {
            newPoints.push_back(this->points[i]);
        }
    }

    this->points = newPoints;
}


