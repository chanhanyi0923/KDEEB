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

            if (line.GetLastPoint() != source.GetLastPoint()) {
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
        if (line.GetPointSize() >= 2) {
            Line source;

            Point prevPoint = line.GetFirstPoint();
            source.AddPoint(prevPoint);

            for (size_t j = 1; j < line.GetPointSize(); j ++) {
                const Point & point = line.GetPoint(j);
                Waypoint waypoint = line.GetWaypointFromPoint(j);

                const double dist = (prevPoint.x - point.x) * (prevPoint.x - point.x) +
                                    (prevPoint.y - point.y) * (prevPoint.y - point.y);

                bool keepPoint = false;
                if (dist > splitDist2 && !(prevPoint.isSegment && point.isSegment)) {
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
                } else if (j == waypoint.closestPointId) {
                    keepPoint = true;
                    // update closest point
                    waypoint.closestPointId = source.GetPointSize();
                }

                if (keepPoint) {
                    source.AddPoint(point);
                    prevPoint = point;
                }
            }

            if (line.GetLastPoint() != source.GetLastPoint()) {
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

            source.AddWaypoints(line);
            source.UpdatePoints();
            this->lines[i] = source;
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
                    if (point.fixed || point.isSegment) {
                        continue;
                    }

                    const Waypoint & waypoint = line.GetWaypointFromPoint(i);

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
    for (size_t i = beginI; i <= endI; i ++) {
        const Point & point = line.GetPoint(i);
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
        line.RemovePointsInSegment();
    }
}


