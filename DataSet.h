#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>

#include "Line.h"

class DataSet
{
    void SmoothShifting(const Line & line, std::vector< std::pair<float, float> > & smoothedPoints, size_t index, size_t indexLowerBound, size_t indexUpperBound, const double interp);

public:
	std::vector<Line> lines;

	DataSet();
	DataSet(size_t size);
	~DataSet();

	friend std::istream & operator >> (std::istream & input, DataSet & dataSet);
	friend std::ostream & operator << (std::ostream & output, const DataSet & dataSet);

	void AddRemovePoints(double removeDist, double splitDist);
    void SmoothTrails(const double interp);

    void AddRemovePointsWithWaypoint(double removeDist, double splitDist);
    void SmoothTrailsWithWaypoint(const double interp);
    void RemovePointsInSegment();
};

