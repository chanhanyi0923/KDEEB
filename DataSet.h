#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>

#include "Line.h"

class DataSet
{
public:
	std::vector<Line> lines;

	DataSet();
	DataSet(size_t size);
	~DataSet();

	friend std::istream & operator >> (std::istream & input, DataSet & dataSet);
	friend std::ostream & operator << (std::ostream & output, const DataSet & dataSet);

	void AddRemovePoints(double removeDist, double splitDist);
    void SmoothTrails(const double interp);
};

