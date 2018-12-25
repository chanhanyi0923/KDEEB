#include "GridGraph.h"
#include <algorithm>
#include <set>

size_t GridGraph::GetRowSize() { return this->rowSize; }

size_t GridGraph::GetColumnSize() { return this->columnSize; }

void GridGraph::SetMaxFlow(int maxFlow) { this->maxFlow = maxFlow; }

void GridGraph::SetGridSize(size_t rowSize, size_t columnSize) {
  this->rowSize = rowSize;
  this->columnSize = columnSize;
  this->grid.resize(rowSize);
  this->overlap.resize(rowSize);
  for (auto &g : this->grid) {
    g.resize(columnSize);
  }
  for (auto &g : this->overlap) {
    g.resize(columnSize, false);
  }
}

void GridGraph::SetTripSize(size_t tripSize) { this->trips.resize(tripSize); }

void GridGraph::AddPoint(size_t tripId, size_t x, size_t y) {
  this->trips[tripId].push_back(std::make_pair(x, y));
}

std::vector<std::pair<int, int> > GridGraph::DrawLine(int x0, int y0, int x1,
                                                      int y1) {
  std::vector<std::pair<int, int> > result;
  bool reverse = false, swap_xy = false;
  if (std::abs(y1 - y0) > std::abs(x1 - x0)) {
    std::swap(x0, y0);
    std::swap(x1, y1);
    swap_xy = true;
  }
  if (x0 > x1) {
    std::swap(x0, x1);
    std::swap(y0, y1);
    reverse = true;
  }

  int dx, dy, p, x, y;

  dx = x1 - x0;
  dy = std::abs(y1 - y0);

  x = x0;
  y = y0;

  p = dx;
  int ystep = y1 > y0 ? 1 : -1;

  while (x <= x1) {
    result.push_back(swap_xy ? std::make_pair(y, x) : std::make_pair(x, y));
    p = p - 2 * dy;
    if (p < 0) {
      y = y + ystep;
      p = p + 2 * dx;

      if (x <= x1) {
        result.push_back(swap_xy ? std::make_pair(y, x) : std::make_pair(x, y));
      }
    }
    x = x + 1;
  }

  if (reverse) {
    std::reverse(result.begin(), result.end());
  }

  return result;
}

void GridGraph::ExpandGridTrip(std::vector<std::pair<int, int> > &trip) {
  std::vector<std::pair<int, int> > result;

  for (size_t i = 1; i < trip.size(); i++) {
    const auto &pointOne = trip[i - 1];
    const auto &pointTwo = trip[i];

    const size_t pointOneX = pointOne.first;
    const size_t pointOneY = pointOne.second;
    const size_t pointTwoX = pointTwo.first;
    const size_t pointTwoY = pointTwo.second;

    const auto &line =
        this->DrawLine(pointOneX, pointOneY, pointTwoX, pointTwoY);
    for (size_t j = 0; j < line.size(); j++) {
      result.push_back(line[j]);
    }
  }
  trip = result;
}

void GridGraph::ReverseTrips() {
  typedef std::pair<std::pair<int, int>, std::pair<int, int> > OdPair;
  std::set<OdPair> odPairs;
  for (auto &trip : this->trips) {
    if (trip.empty()) {
      continue;
    }
    int sx = trip.front().first;
    int sy = trip.front().second;
    int tx = trip.back().first;
    int ty = trip.back().second;

    OdPair st(std::make_pair(sx, sy), std::make_pair(tx, ty));
    OdPair ts(std::make_pair(tx, ty), std::make_pair(sx, sy));

    if (odPairs.find(ts) != odPairs.end()) {
      reverse(trip.begin(), trip.end());
    } else {
      odPairs.insert(st);
    }
  }
}

void GridGraph::GetCapacity() {
  for (const auto &trip : this->trips) {
    if (trip.empty()) {
      continue;
    }

    for (size_t j = 1; j < trip.size(); j++) {
      int x0 = trip[j - 1].first;
      int y0 = trip[j - 1].second;
      int x1 = trip[j].first;
      int y1 = trip[j].second;
      Grid &g = grid[x0][y0];

      if (x0 != x1 && y0 != y1) {
        throw "grid data error";
      }
      if (x0 == x1 && y0 == y1) {
        continue;
      }
      if (x0 == x1) {
        if (y0 < y1) {
          g.up++;
        } else {
          g.down++;
        }
      } else {
        if (x0 < x1) {
          g.right++;
        } else {
          g.left++;
        }
      }
    }
    grid[trip.front().first][trip.front().second].isSource = true;
    grid[trip.back().first][trip.back().second].isSink = true;
  }
}

void GridGraph::RemoveOverlap() {
  for (int i = 0; i < this->rowSize; i++) {
    for (int j = 0; j < this->columnSize; j++) {
      Grid &g = grid[i][j];
      if (g.isSource && g.isSink) {
        overlap[i][j] = true;
      }
    }
  }
}

void GridGraph::ConvertToGrid() {
  for (auto &trip : this->trips) {
    this->ExpandGridTrip(trip);
  }
  this->ReverseTrips();
  this->GetCapacity();
}

int GridGraph::FlowToCapacity(int flow) {
  // const int multiply = 10000;
  if (flow == 0) {
    return 0;
  } else {
    return this->maxFlow / flow;
  }
}

int GridGraph::GetLeftCapacity(int x, int y) {
  // int leftWeight = std::max(grid[i][j].left > 0 ? 1u : 0u,
  // weightToInteger(grid[i][j].left) - rate * turn[i][j] / trips_size);
  if (overlap[x][y]) {
    return 1;
  } else {
    return this->FlowToCapacity(grid[x][y].left);
  }
}

int GridGraph::GetRightCapacity(int x, int y) {
  if (overlap[x][y]) {
    return 1;
  } else {
    return this->FlowToCapacity(grid[x][y].right);
  }
}

int GridGraph::GetDownCapacity(int x, int y) {
  if (overlap[x][y]) {
    return 1;
  } else {
    return this->FlowToCapacity(grid[x][y].down);
  }
}

int GridGraph::GetUpCapacity(int x, int y) {
  if (overlap[x][y]) {
    return 1;
  } else {
    return this->FlowToCapacity(grid[x][y].up);
  }
}

int GridGraph::GetSourceCapacity(int x, int y) {
  if (overlap[x][y]) {
    return 1;
  } else {
    return this->FlowToCapacity(grid[x][y].isSource ? 1 : 0);
  }
}

int GridGraph::GetSinkCapacity(int x, int y) {
  if (overlap[x][y]) {
    return 1;
  } else {
    return this->FlowToCapacity(grid[x][y].isSink ? 1 : 0);
  }
}
