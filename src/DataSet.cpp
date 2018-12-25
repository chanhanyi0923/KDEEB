#include "DataSet.h"


DataSet::DataSet()
{

}


DataSet::DataSet(size_t size)
{

    this->lines.resize(size);
}


DataSet::~DataSet()
{
}


std::istream & operator >> (std::istream & input, DataSet & dataSet)
{
    using std::string;
    using std::istringstream;

    size_t countLine = 0;
    for (int c; !input.eof(); ) {
        c = input.get();
        if (c == '\n') {
            countLine++;
        }
    }

    // alloc memory
    dataSet.lines.resize(countLine);

    // return to the beginning of input file.
    input.clear();
    input.seekg(0, std::ios::beg);

    // read each line
    for (size_t lineNum = 0; lineNum < countLine; lineNum ++) {
        string stringLine;
        getline(input, stringLine);
        istringstream lineBuffer(stringLine);
        string token;

        double number[4] = { 0.0, 0.0, 0.0, 0.0 };
        // split by ','
        for (int i = 0; i < 4; i++) {
            getline(lineBuffer, token, ',');
            istringstream buffer(token);
            buffer >> number[i];
        }

        Line & line = dataSet.lines[lineNum];
        line.AddPoint(Point(number[1], number[0], 1.0f));
        line.AddPoint(Point(number[3], number[2], 1.0f));
    }
    return input;
}


std::ostream & operator << (std::ostream & output, const DataSet & dataSet)
{
    output << std::setprecision(15);
    for (size_t i = 0; i < dataSet.lines.size(); i ++) {
        const Line & lines = dataSet.lines[i];
        for (size_t j = 0; j < lines.GetPointSize(); j ++) {
            const Point & point = lines.GetPoint(j);
            output << point.x << "," << point.y << "," << (i + 1) << std::endl;
        }
    }
    output << std::resetiosflags(std::ios::showbase);

    return output;
}

#define GRID_WIDTH 0.0003
size_t DataSet::GetGridIndexOfX(double x)
{
	return floor( (x - this->xMin + 0.5 * GRID_WIDTH) / GRID_WIDTH );
}


size_t DataSet::GetGridIndexOfY(double y)
{
	return floor( (y - this->yMin + 0.5 * GRID_WIDTH) / GRID_WIDTH );
}


void DataSet::CreateGridGraph()
{
    gridGraph.SetTripSize(500000);
    gridGraph.SetMaxFlow(500000);
	size_t x_num = 1 + ceil( (this->xMax - this->xMin + GRID_WIDTH) / GRID_WIDTH );
	size_t y_num = 1 + ceil( (this->yMax - this->yMin + GRID_WIDTH) / GRID_WIDTH );
    gridGraph.SetGridSize(x_num, y_num);
    for (int i = 0; i < 500000; i ++) {
        for (const Point & p: rawLines[i]) {
            int x = GetGridIndexOfX(p.x);
            int y = GetGridIndexOfY(p.y);
            gridGraph.AddPoint(i, x, y);
        }
    }
    gridGraph.ConvertToGrid();
    this->minCutSolver.SetData(&this->gridGraph);
}


void DataSet::UpdateWaypoints(const std::vector<Point> &refPoints)
{
    this->minCutSolver.GetCuts();

    using std::vector;

    for (size_t lineId = 0; lineId < this->rawLines.size(); lineId ++) {
        auto & rawLine = this->rawLines[lineId];
        if (rawLine.empty()) {
            continue;
        }

        for (size_t i = 0; i < rawLine.size(); i ++) {
            Point & point = rawLine[i];
            if (point.fixed) {
                point.prevFixedPointId = i;
            } else {
                if (i == 0) {
                    point.prevFixedPointId = -1;
                } else {
                    point.prevFixedPointId = rawLine[i - 1].prevFixedPointId;
                }
            }
        }
        
        for (size_t i = rawLine.size() - 1; i != (size_t)-1; i --) {
            Point & point = rawLine[i];
            if (point.fixed) {
                point.nextFixedPointId = i;
            } else {
                if (i == rawLine.size() - 1) {
                    point.nextFixedPointId = -1;
                } else {
                    point.nextFixedPointId = rawLine[i + 1].nextFixedPointId;
                }
            }
        }

        for (size_t i = 0, j = 0; i < rawLine.size(); i ++) {
            Point & point = rawLine[i];
            int x = this->GetGridIndexOfX(point.x);
            int y = this->GetGridIndexOfY(point.y);

            bool isCut = false;
            for (; j < gridGraph.trips[lineId].size(); j ++) {
                int x_ = gridGraph.trips[lineId][j].first;
                int y_ = gridGraph.trips[lineId][j].second;

                if (minCutSolver.IsCut(x_, y_)) {
                    isCut = true;
                }

                if (x_ == x && y_ == y) {
                    j ++;
                    break;
                }
            }

            if (isCut) {
                point.fixed = true;
                const Point prevP = rawLine[point.prevFixedPointId];
                const Point nextP = rawLine[point.nextFixedPointId];

                Line & line = this->lines[lineId];
                const size_t oId = line.FindPointIndexById(prevP.id);
                const size_t dId = line.FindPointIndexById(nextP.id);
                if (oId != (size_t)-1 && dId != (size_t)-1) {
                    const Point &rp = refPoints[point.id];
                    Waypoint wp(rp.x, rp.y, rp.id);
                    line.AddWaypoint(oId, dId, wp);
                }
            }
        }
    }
}


void DataSet::AddRemovePoints(double removeDist, double splitDist)
{
    const double removeDist2 = removeDist * removeDist;
    const double splitDist2 = splitDist * splitDist;

    #pragma omp parallel for
    for (size_t i = 0; i < this->lines.size(); i ++) {
        const Line & line = this->lines[i];
        if (line.GetPointSize() >= 2) {
            Line source;

            Point prevPoint = line.GetFirstPoint();
            source.AddPoint(prevPoint);

            for (size_t j = 1; j < line.GetPointSize(); j ++) {
                const Point & point = line.GetPoint(j);

                const double dist = (prevPoint.x - point.x) * (prevPoint.x - point.x) +
                                    (prevPoint.y - point.y) * (prevPoint.y - point.y);

                if (dist > splitDist2) {
                    source.AddPoint(Point(
                        (prevPoint.x + point.x) / 2.0,
                        (prevPoint.y + point.y) / 2.0,
                        (prevPoint.z + point.z) / 2.0,
                        point.timeNormelized,
                        point.timeInt
                    ));
                    source.AddPoint(point);
                    prevPoint = point;
                } else if (dist > removeDist2) {
                    source.AddPoint(point);
                    prevPoint = point;
                }
            }

            if ( !line.GetLastPoint().CompareId( source.GetLastPoint() ) ) {
                source.AddPoint(line.GetLastPoint());
            }


            const double firstTimeInt = source.GetFirstPoint().timeInt;
            const double lastTimeInt = source.GetLastPoint().timeInt;
            const double timeLength = (lastTimeInt - firstTimeInt) / (source.GetPointSize() - 1);
            for (size_t k = 0; k < source.GetPointSize(); k++) {
                Point point = source.GetPoint(k);
                point.timeInt = firstTimeInt + timeLength * k;
                source.SetPoint(k, point);
            }

            this->lines[i] = source;
        }
    }
}



void DataSet::AddRemovePointsWithWaypoint(double removeDist, double splitDist)
{
    const double removeDist2 = removeDist * removeDist;
    const double splitDist2 = splitDist * splitDist;

    #pragma omp parallel for
    for (size_t i = 0; i < this->lines.size(); i ++) {
        const Line & line = this->lines[i];

        try {

        if (line.GetPointSize() >= 2) {
            Line source;

            Point prevPoint = line.GetFirstPoint();
            source.AddPoint(prevPoint);

            for (size_t j = 1; j < line.GetPointSize(); j ++) {
                const Point & point = line.GetPoint(j);

                const double dist = (prevPoint.x - point.x) * (prevPoint.x - point.x) +
                                    (prevPoint.y - point.y) * (prevPoint.y - point.y);

                bool keepPoint = false;
                if (dist > splitDist2 && !(prevPoint.isSegment && point.isSegment) ) {
                    Point newPoint(
                        (prevPoint.x + point.x) / 2.0,
                        (prevPoint.y + point.y) / 2.0,
                        (prevPoint.z + point.z) / 2.0,
                        point.timeNormelized,
                        point.timeInt
                    );
/*
                    if (j + 1 < line.GetPointSize()) {
                        const Point & nextPoint = line.GetPoint(j + 1);
                        if (prevPoint.isSegment && nextPoint.isSegment) {
                            newPoint.isSegment = true;
                        }
                    }
*/
                    source.AddPoint(newPoint);
                    keepPoint = true;
                } else if (dist > removeDist2) {
                    keepPoint = true;
                } else if (point.fixed) {
                    keepPoint = true;
                } else if (point.waypointId != (size_t)-1) {
                    Waypoint waypoint = line.GetWaypointFromPointId(j);

                    if (j == waypoint.closestPointId) {
                        keepPoint = true;
                        // update closest point
                        waypoint.closestPointId = source.GetPointSize();
                    }
                }

                if (keepPoint) {
                    source.AddPoint(point);
                    prevPoint = point;
                }
            }

            if ( !line.GetLastPoint().CompareId( source.GetLastPoint() ) ) {
//            if (line.GetLastPoint() != source.GetLastPoint()) {
                source.AddPoint(line.GetLastPoint());
            }

            const double firstTimeInt = source.GetFirstPoint().timeInt;
            const double lastTimeInt = source.GetLastPoint().timeInt;
            const double timeLength = (lastTimeInt - firstTimeInt) / (source.GetPointSize() - 1);
            for (size_t k = 0; k < source.GetPointSize(); k++) {
                Point point = source.GetPoint(k);
                point.timeInt = firstTimeInt + timeLength * k;
                source.SetPoint(k, point);
            }

            source.id = line.id;
            source.segments = line.segments;
            source.AddWaypoints(line);
            source.UpdatePoints();
            this->lines[i] = source;
        }

        } catch (...) {
            this->lines[i] = Line();
        }
    }
}





void DataSet::SmoothTrails(const double interp)
{
    using std::vector;
    using std::pair;

    #pragma omp parallel for
    for (size_t _ = 0; _ < this->lines.size(); _ ++) {
        Line & line = this->lines[_];
        const size_t lineSize = line.GetPointSize();

        if (lineSize >= 2) {
            vector< pair<float, float> > smoothedPoints(lineSize);

            for (size_t i = 1; i < lineSize - 1; i ++) {
                const Point & point = line.GetPoint(i);
                smoothedPoints[i] = std::make_pair(point.x, point.y);
            }

            const size_t iterationCount = 10;
            for (size_t count = 0; count < iterationCount; count ++) {
                for (size_t i = 1; i < lineSize - 1; i ++) {

                    double sumX = 0.0;
                    double sumY = 0.0;
                    size_t nearPointsSize = 0;
                    for (int k = -4; k <= 4; k++) {
                        int index = i + k;

                        // check if the index is legal
                        if (index >= 0 && index < (int)lineSize) {
                            const Point & point = line.GetPoint(index);
                            sumX += point.x;
                            sumY += point.y;
                            nearPointsSize ++;
                        }
                    }
                    sumX /= nearPointsSize;
                    sumY /= nearPointsSize;

                    smoothedPoints[i].first = smoothedPoints[i].first * (1.0 - interp) + sumX * interp;
                    smoothedPoints[i].second = smoothedPoints[i].second * (1.0 - interp) + sumY * interp;
                }
                for (size_t i = 1; i < lineSize - 1; i ++) {
                    Point point = line.GetPoint(i);
                    point.x = smoothedPoints[i].first;
                    point.y = smoothedPoints[i].second;
                    line.SetPoint(i, point);
                }
            }

        }
    }
}


void DataSet::SmoothTrailsWithWaypoint(const double interp)
{
    using std::vector;
    using std::pair;

    #pragma omp parallel for
    for (size_t _ = 0; _ < this->lines.size(); _ ++) {
        Line & line = this->lines[_];
        const size_t lineSize = line.GetPointSize();

        if (lineSize >= 2) {
            vector< pair<float, float> > smoothedPoints(lineSize);

            for (size_t i = 1; i < lineSize - 1; i ++) {
                const Point & point = line.GetPoint(i);
                smoothedPoints[i] = std::make_pair(point.x, point.y);
            }

            const size_t iterationCount = 10;
            for (size_t count = 0; count < iterationCount; count ++) {
                for (size_t i = 1; i < lineSize - 1; i ++) {
                    const Point & point = line.GetPoint(i);
                    if (point.fixed/* || point.isSegment*/) {
                        continue;
                    }

                    const Waypoint & waypoint = line.GetWaypointFromPointId(i);

                    const size_t closestPointId = waypoint.closestPointId;

                    if (closestPointId == (size_t)-1) {
                        // need to be changed
                        std::cout << " " << std::endl;

                        std::cout << closestPointId << std::endl;
                        throw "!";
                    }

                    if (i < closestPointId) {
                        this->SmoothShifting(line, smoothedPoints, i, 0, closestPointId, interp);
                    } else if (i > closestPointId) {
                        this->SmoothShifting(line, smoothedPoints, i, closestPointId, lineSize - 1, interp);
                    }
                }
                for (size_t i = 1; i < lineSize - 1; i ++) {
                    Point point = line.GetPoint(i);
                    if (!point.fixed) {
                        point.x = smoothedPoints[i].first;
                        point.y = smoothedPoints[i].second;
                    }
                    line.SetPoint(i, point);
                }
            }
        }
    }
}


void DataSet::SmoothShifting(const Line & line, std::vector< std::pair<float, float> > & smoothedPoints, size_t index, size_t indexLowerBound, size_t indexUpperBound, const double interp)
{
    const int smoothRange = 4;

    double nearPointsAvgX = 0.0;
    double nearPointsAvgY = 0.0;
    size_t nearPointsSize = 0;

    const size_t beginI = std::max((int)indexLowerBound, (int)index - smoothRange);
    const size_t endI = std::min((int)indexUpperBound, (int)index + smoothRange);

    for (size_t i = index; i >= beginI; i --) {
        const Point & point = line.GetPoint(i);
        if (point.fixed) {
            break;
        }
        nearPointsAvgX += point.x;
        nearPointsAvgY += point.y;
        nearPointsSize ++;
    }

    for (size_t i = index; i <= endI; i ++) {
        const Point & point = line.GetPoint(i);
        if (point.fixed) {
            break;
        }
        nearPointsAvgX += point.x;
        nearPointsAvgY += point.y;
        nearPointsSize ++;
    }

    nearPointsAvgX /= nearPointsSize;
    nearPointsAvgY /= nearPointsSize;

    smoothedPoints[index].first = smoothedPoints[index].first * (1.0 - interp) + nearPointsAvgX * interp;
    smoothedPoints[index].second = smoothedPoints[index].second * (1.0 - interp) + nearPointsAvgY * interp;
}


void DataSet::RemovePointsInSegment()
{
    #pragma omp parallel for
    for (size_t i = 0; i < this->lines.size(); i ++) {
        Line & line = this->lines[i];
        try {
            line.RemovePointsInSegment();
        } catch (...) {
            line = Line();
        }
    }
}


