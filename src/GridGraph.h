#pragma once

#include <cstddef>
#include <vector>

class GridGraph {
 private:
  class Grid {
   public:
    bool isSource, isSink;
    size_t left, right, up, down;
    Grid()
        : left(0), right(0), up(0), down(0), isSource(false), isSink(false) {}
  };

  int maxFlow;
  size_t rowSize, columnSize;

  std::vector<std::vector<bool> > overlap;

  size_t getGridX(double x);
  size_t getGridY(double y);
  std::vector<std::pair<int, int> > DrawLine(int x0, int y0, int x1, int y1);
  void ReverseTrips();
  void GetCapacity();
  void RemoveOverlap();
  int FlowToCapacity(int flow);
  void ExpandGridTrip(std::vector<std::pair<int, int> > &trip);

 public:
  std::vector<std::vector<std::pair<int, int> > > trips;
  std::vector<std::vector<Grid> > grid;

  size_t GetRowSize();
  size_t GetColumnSize();
  void SetMaxFlow(int maxFlow);
  void SetGridSize(size_t rowSize, size_t columnSize);
  void SetTripSize(size_t tripSize);
  void AddPoint(size_t tripId, size_t x, size_t y);

  void ConvertToGrid();
  int GetLeftCapacity(int x, int y);
  int GetRightCapacity(int x, int y);
  int GetDownCapacity(int x, int y);
  int GetUpCapacity(int x, int y);
  int GetSourceCapacity(int x, int y);
  int GetSinkCapacity(int x, int y);
};
