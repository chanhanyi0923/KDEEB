#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>

#include "Line.h"
#include "GridGraph.h"
#include "MinCutSolver.hpp"

class DataSet {
 private:
  MinCutSolver<int> minCutSolver;
  GridGraph gridGraph;
  //const int gridWidth = 0.0003;
  int gridWidth;

  size_t GetGridIndexOfX(double x);
  size_t GetGridIndexOfY(double y);

  void SmoothShifting(const Line &line,
                      std::vector<std::pair<float, float> > &smoothedPoints,
                      size_t index, size_t indexLowerBound,
                      size_t indexUpperBound, const double interp);

 public:
  double xMin, xMax, yMin, yMax;
  std::vector<Line> lines;
  std::vector<std::vector<Point> > rawLines;

  DataSet();
  DataSet(size_t size);
  ~DataSet();

  friend std::istream &operator>>(std::istream &input, DataSet &dataSet);
  friend std::ostream &operator<<(std::ostream &output, const DataSet &dataSet);

  void AddRemovePoints(double removeDist, double splitDist);
  void SmoothTrails(const double interp);
  void CreateGridGraph();
  void UpdateWaypoints(const std::vector<Point> &refPoints);
  void AddRemovePointsWithWaypoint(double removeDist, double splitDist);
  void SmoothTrailsWithWaypoint(const double interp);
  void RemovePointsInSegment();
};
