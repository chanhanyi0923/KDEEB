#include "Line.h"

// debug
#include <iostream>
//
#include <limits>
#include <cmath>

Line::Line():
    static_(false)
{
}


Line::~Line()
{
}


void Line::AddSecondLastPoint(const Point & point)
{
    Point back = this->points.back();
    this->points.pop_back();
    this->points.push_back(point);
    this->points.push_back(back);
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

    for (Point & point : this->points) {
        point.preparedWaypoint = false;
    }
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
        //std::cout << "line id: " << this->id << std::endl;

        //throw "index = -1";
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


void Line::AddWaypoint(const size_t oId_, const size_t dId_, Waypoint waypoint)
{
    const size_t oId = (dId_ < oId_) ? dId_ : oId_;
    const size_t dId = (dId_ < oId_) ? oId_ : dId_;
//    if (dId < oId) {
//        throw "!!!";
//        std::swap(oId, dId);
//    }

    waypoint.oId = oId;
    waypoint.dId = dId;

    Interval & interval = this->intervals[ std::make_pair(waypoint.oId, waypoint.dId) ];
    interval.lowerBound = 0;

    if (this->GetPointSize() <= 2) {
        this->points.clear();
        this->waypoints.clear();
        this->intervals.clear();
        this->segments.clear();
        //throw "number of points in line is too few";
    } else if (this->GetPointSize() > 2) {
        double minDist2 = std::numeric_limits<double>::infinity();

        const size_t beginI = std::max(waypoint.oId, interval.lowerBound);
        const size_t endI = std::min(waypoint.dId, this->GetPointSize());
        for (size_t i = beginI; i <= endI; i ++) {
            const Point & point = this->GetPoint(i);

            // skip fixed points
            if (point.fixed || point.preparedWaypoint) {
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
    const size_t waypointId = this->waypoints.size() - 1;

    if (waypoint.closestPointId != (size_t)-1) {
        const size_t pointId = waypoint.closestPointId;
        Point point = this->GetPoint(pointId);
        point.preparedWaypoint = true;
        point.waypointId = waypointId;
        this->SetPoint(pointId, point);
    } else {
        throw "number of points in line is too few";
    }

    interval.waypointIds.push_back(waypointId);
}


void Line::UpdateWaypointInfo(const Line & line)
{
    this->waypoints = line.waypoints;
    for (Waypoint & waypoint: this->waypoints) {
        // do not use waypoint.oId, waypoint.dId, since they are not updated
        const Point & o = line.GetPoint(waypoint.oId);
        const Point & d = line.GetPoint(waypoint.dId);

        // this is the real index in current line
        const size_t oId = this->FindPointIndexById(o.id);
        const size_t dId = this->FindPointIndexById(d.id);

        waypoint.oId = oId;
        waypoint.dId = dId;


        const size_t pointId = waypoint.closestPointId;
        const Point & closestPoint = this->points[pointId];
        waypoint.minDist = std::sqrt(
            (closestPoint.x - waypoint.x) * (closestPoint.x - waypoint.x) +
            (closestPoint.y - waypoint.y) * (closestPoint.y - waypoint.y)
        );
    }
}

void Line::UpdatePoints()
{
    if (this->waypoints.empty()) {
        this->static_ = true;
        return;
    } else {
        this->static_ = false;
    }

    std::vector<size_t> prevPreparedWaypointIds(this->points.size());
    std::vector<size_t> prevFixedIds(this->points.size());
    for (size_t i = 0; i < this->points.size(); i ++) {
        const Point & point = this->points[i];

        if (point.fixed) {
            prevFixedIds[i] = i;
        } else {
            prevFixedIds[i] = prevFixedIds[i - 1];
        }

        if (point.preparedWaypoint) {
            prevPreparedWaypointIds[i] = i;
        } else {
            if (i == 0) {
                prevPreparedWaypointIds[i] = (size_t)(-1);
            } else {
                prevPreparedWaypointIds[i] = prevPreparedWaypointIds[i - 1];
            }
        }
    }

    std::vector<size_t> nextPreparedWaypointIds(this->points.size());
    std::vector<size_t> nextFixedIds(this->points.size());
    for (int i = (int)this->points.size() - 1; i >= 0; i --) {
        const Point & point = this->points[i];

        if (point.fixed) {
            nextFixedIds[i] = i;
        } else {
            nextFixedIds[i] = nextFixedIds[i + 1];
        }

        if (point.preparedWaypoint) {
            nextPreparedWaypointIds[i] = i;
        } else {
            if (i == (int)this->points.size() - 1) {
                nextPreparedWaypointIds[i] = (size_t)(-1);
            } else {
                nextPreparedWaypointIds[i] = nextPreparedWaypointIds[i + 1];
            }
        }
    }

    for (size_t i = 0; i < this->points.size(); i ++) {
        Point & point = this->points[i];
        if (point.fixed) {
            //
        } else if (point.preparedWaypoint) {
            Waypoint & waypoint = this->waypoints[point.waypointId];
            waypoint.closestPointId = i;
        } else {
            if (prevPreparedWaypointIds[i] != (size_t)-1 && nextPreparedWaypointIds[i] != (size_t)-1) {
                size_t preparedWaypointId;
                if (i - prevPreparedWaypointIds[i] < nextPreparedWaypointIds[i] - i) {
                    preparedWaypointId = prevPreparedWaypointIds[i];
                } else {
                    preparedWaypointId = nextPreparedWaypointIds[i];
                }
                const Point & preparedWaypoint = this->points[preparedWaypointId];
                point.waypointId = preparedWaypoint.waypointId;
                point.prevFixedPointId = std::max(prevPreparedWaypointIds[i], prevFixedIds[i]);
                point.nextFixedPointId = std::min(nextPreparedWaypointIds[i], nextFixedIds[i]);
            } else if (prevPreparedWaypointIds[i] != (size_t)-1) {
                size_t preparedWaypointId = prevPreparedWaypointIds[i];
                const Point & preparedWaypoint = this->points[preparedWaypointId];

                point.waypointId = preparedWaypoint.waypointId;
                point.prevFixedPointId = std::max(prevPreparedWaypointIds[i], prevFixedIds[i]);
                point.nextFixedPointId = nextFixedIds[i];
            } else if (nextPreparedWaypointIds[i] != (size_t)-1) {
                size_t preparedWaypointId = nextPreparedWaypointIds[i];
                const Point & preparedWaypoint = this->points[preparedWaypointId];

                point.waypointId = preparedWaypoint.waypointId;
                point.prevFixedPointId = prevFixedIds[i];
                point.nextFixedPointId = std::min(nextPreparedWaypointIds[i], nextFixedIds[i]);
            } else {
                throw "no appropriate waypoint";
            }
        }
    }
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
        if (oId >= this->points.size() || dId >= this->points.size() ||
            oId == (size_t)-1 || dId == (size_t)-1) {
          continue;
        }
        if (oId > dId) {
            std::swap(oId, dId);
        }
        this->points[oId].isSegment = true;
        this->points[dId].isSegment = true;
        if (oId < dId) {
          for (size_t i = oId + 1; i <= dId - 1; i ++) {
              keep[i] = false;
          }
        }
    }

    for (size_t i = 0; i < this->points.size(); i ++) {
        if (keep[i]) {
            newPoints.push_back(this->points[i]);
        }
    }

    this->points = newPoints;
}


void Line::PrintFixedPoints() {
    if (!this->points.empty()) {
        for (const auto &point: this->points) {
            if (point.id != -1) {
                printf("%d ", point.id);
            }
        }
        printf("\n");
    }
}


// void Line::SetWaypointToFixed()
// {
//     for (Point &point: this->points) {
//         if (point.preparedWaypoint) {
//             if (point.waypointId == (size_t)-1) {
//                 throw "waypoint id = -1";
//             }
//             const Waypoint &waypoint = this->waypoints[point.waypointId];
//             point.id = waypoint.id;
//             point.fixed = true;
//             point.preparedWaypoint = false;
//         }
//     }
// }



void Line::AddPointsForWaypoint(const std::vector<Waypoint> & waypoints)
{
    if (this->GetPointSize() < 2) {
        return;
    }

    std::map<size_t, size_t> indices;
    std::map<size_t, size_t> addCountLeft;
    std::map<size_t, size_t> addCountRight;

    for (size_t i = 0; i < this->points.size(); i ++) {
        const Point &point = this->points[i];
        if (point.id != (size_t)-1) {
            if (indices.find(point.id) == indices.end()) {
                indices[point.id] = i;
            }
        }
    }

    for (const Waypoint & waypoint: waypoints) {
        size_t oId = waypoint.oId;
        size_t dId = waypoint.dId;
        if (indices[oId] < indices[dId]) {
            addCountLeft[dId] += 2;
            addCountRight[oId] += 2;
        } else {
            addCountLeft[oId] += 2;
            addCountRight[dId] += 2;
        }
    }

    std::vector<Point> newPoints;
    for (size_t j = 0; j < this->GetPointSize(); j ++) {
        const Point & point = this->GetPoint(j);

        if (j != 0) {
            if (point.id != (size_t)-1) {
                const auto iter = addCountLeft.find(point.id);
                if (iter != addCountLeft.end()) {
                    const size_t count = iter->second;
                    for (size_t k = 0; k < count; k ++) {
                        Point newPoint(point.x, point.y, 0);
                        newPoints.push_back(newPoint);
                    }
                }
            }
        }

        if (!(point.id != (size_t)-1 && indices[point.id] != j)) {
            newPoints.push_back(point);
        }

        if (j != this->GetPointSize() - 1) {
            if (point.id != (size_t)-1) {
                const auto iter = addCountRight.find(point.id);
                if (iter != addCountRight.end()) {
                    const size_t count = iter->second;
                    for (size_t k = 0; k < count; k ++) {
                        Point newPoint(point.x, point.y, 0);
                        newPoints.push_back(newPoint);
                    }
                }
            }
        }
    }

    this->points = newPoints;


    // if (this->GetPointSize() < 2) {
    //     return;
    // }

    // std::map<size_t, size_t> addCount;

    // for (const Waypoint & waypoint: waypoints) {
    //     size_t oId = waypoint.oId;
    //     size_t dId = waypoint.dId;
    //     addCount[oId] += 2;
    //     addCount[dId] += 2;
    // }

    // std::vector<Point> newPoints;
    // for (size_t j = 0; j < this->GetPointSize(); j ++) {
    //     const Point & point = this->GetPoint(j);

    //     if (j != 0) {
    //         if (point.id != (size_t)-1 && !(this->points[j - 1].isSegment && point.isSegment)) {
    //             const auto iter = addCount.find(point.id);
    //             if (iter != addCount.end()) {
    //                 const size_t count = iter->second;
    //                 for (size_t k = 0; k < count; k ++) {
    //                     Point newPoint(point.x, point.y, 0);
    //                     newPoints.push_back(newPoint);
    //                 }
    //             }
    //         }
    //     }

    //     newPoints.push_back(point);

    //     if (j != this->GetPointSize() - 1) {
    //         if (point.id != (size_t)-1 && (this->points[j + 1].isSegment && point.isSegment)) {
    //             const auto iter = addCount.find(point.id);
    //             if (iter != addCount.end()) {
    //                 const size_t count = iter->second;
    //                 for (size_t k = 0; k < count; k ++) {
    //                     Point newPoint(point.x, point.y, 0);
    //                     newPoints.push_back(newPoint);
    //                 }
    //             }
    //         }
    //     }
    // }

    // this->points = newPoints;
}

