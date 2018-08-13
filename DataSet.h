#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>

#include "Record.h"

using std::istream;
using std::ostream;
using std::vector;

class DataSet
{
public:
	vector< vector<Record> > data;

	DataSet();
	DataSet(size_t size);
	virtual ~DataSet();

	friend istream & operator >> (istream & input, DataSet & dataSet);
	friend ostream & operator << (ostream & output, const DataSet & dataSet);

	void AddRemovePoints(double removeDist, double splitDist);
    void SmoothTrails(const double interp);
};

