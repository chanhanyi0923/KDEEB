#pragma once

#include <vector>
#include <queue>
#include <limits>
#include <GridCut/GridGraph_2D_4C_MT.h>
#include "GridGraph.h"

template <typename cap_t>
class MinCutSolver {
 private:
  typedef GridGraph_2D_4C_MT<cap_t, cap_t, cap_t> Grid;

  std::vector<cap_t> source, sink, left, right, down, up;
  std::vector<bool> result, isRoad, isCut;//, prevIsCut;
  int rowSize, columnSize;
  Grid *grid;

  void WriteBack() {
    for (int i = 0; i < rowSize; i++) {
      for (int j = 0; j < columnSize; j++) {
        const int node_id = grid->node_id(j, i);  // j = x, i = y
        const size_t index = i * columnSize + j;
        result[index] = grid->get_segment(node_id);
      }
    }
  }

  void BfsRemove(size_t x, size_t y, int w, bool r /* result */) {
    if (w == 0) {
      return;
    }

    std::vector<std::pair<size_t, size_t> > V;
    std::queue<std::pair<size_t, size_t> > Q;
    Q.push(std::make_pair(x, y));
    while (!Q.empty()) {
      V.push_back(Q.front());
      size_t i = Q.front().first;
      size_t j = Q.front().second;
      Q.pop();

      const size_t index = i * columnSize + j;
      const size_t indexLeft = (i - 1) * columnSize + j;
      const size_t indexRight = (i + 1) * columnSize + j;
      const size_t indexDown = i * columnSize + j - 1;
      const size_t indexUp = i * columnSize + j + 1;

      if (r) {
        if (i != 0 && result[indexLeft] && isRoad[indexLeft] &&
            left[index] == w) {
          left[index] = 0;
          right[indexLeft] = 0;
          Q.push(std::make_pair(i - 1, j));
        }
        if (i != rowSize - 1 && result[indexRight] && isRoad[indexRight] &&
            right[index] == w) {
          right[index] = 0;
          left[indexRight] = 0;
          Q.push(std::make_pair(i + 1, j));
        }
        if (j != 0 && result[indexDown] && isRoad[indexDown] &&
            down[index] == w) {
          down[index] = 0;
          up[indexDown] = 0;
          Q.push(std::make_pair(i, j - 1));
        }
        if (j != columnSize - 1 && result[indexUp] && isRoad[indexUp] &&
            up[index] == w) {
          up[index] = 0;
          down[indexUp] = 0;
          Q.push(std::make_pair(i, j + 1));
        }
      } else {
        if (i != 0 && !result[indexLeft] && isRoad[indexLeft] &&
            right[indexLeft] == w) {
          left[index] = 0;
          right[indexLeft] = 0;
          Q.push(std::make_pair(i - 1, j));
        }
        if (i != rowSize - 1 && !result[indexRight] && isRoad[indexRight] &&
            left[indexRight] == w) {
          right[index] = 0;
          left[indexRight] = 0;
          Q.push(std::make_pair(i + 1, j));
        }
        if (j != 0 && !result[indexDown] && isRoad[indexDown] &&
            up[indexDown] == w) {
          down[index] = 0;
          up[indexDown] = 0;
          Q.push(std::make_pair(i, j - 1));
        }
        if (j != columnSize - 1 && !result[indexUp] && isRoad[indexUp] &&
            down[indexUp] == w) {
          up[index] = 0;
          down[indexUp] = 0;
          Q.push(std::make_pair(i, j + 1));
        }
      }
    }

    for (const auto &v : V) {
      size_t i = v.first;
      size_t j = v.second;
      const size_t index = i * columnSize + j;
      const size_t indexLeft = (i - 1) * columnSize + j;
      const size_t indexRight = (i + 1) * columnSize + j;
      const size_t indexDown = i * columnSize + j - 1;
      const size_t indexUp = i * columnSize + j + 1;

      if (result[index]) {
        if (left[index] > 0 || right[index] > 0 || down[index] > 0 ||
            up[index] > 0) {
          source[index] = std::numeric_limits<cap_t>::max();
        }
      } else {
        if (right[indexLeft] > 0 || left[indexRight] > 0 || up[indexDown] > 0 ||
            down[indexUp] > 0) {
          sink[index] = std::numeric_limits<cap_t>::max();
        }
      }
    }
  }

  void Split() {
    for (int i = 0; i < rowSize; i++) {
      for (int j = 0; j < columnSize; j++) {
        const size_t index = i * columnSize + j;
        const size_t indexLeft = (i - 1) * columnSize + j;
        const size_t indexRight = (i + 1) * columnSize + j;
        const size_t indexDown = i * columnSize + j - 1;
        const size_t indexUp = i * columnSize + j + 1;

        if (result[index]) {
          if (i != 0 && !result[indexLeft] && isRoad[indexLeft]) {
            BfsRemove(i - 1, j, right[indexLeft], false);
          }
          if (i != rowSize - 1 && !result[indexRight] && isRoad[indexRight]) {
            BfsRemove(i + 1, j, left[indexRight], false);
          }
          if (j != 0 && !result[indexDown] && isRoad[indexDown]) {
            BfsRemove(i, j - 1, up[indexDown], false);
          }
          if (j != columnSize - 1 && !result[indexUp] && isRoad[indexUp]) {
            BfsRemove(i, j + 1, down[indexUp], false);
          }
        } else {
          if (i != 0 && result[indexLeft] && isRoad[indexLeft]) {
            BfsRemove(i - 1, j, left[index], true);
          }
          if (i != rowSize - 1 && result[indexRight] && isRoad[indexRight]) {
            BfsRemove(i + 1, j, right[index], true);
          }
          if (j != 0 && result[indexDown] && isRoad[indexDown]) {
            BfsRemove(i, j - 1, down[index], true);
          }
          if (j != columnSize - 1 && result[indexUp] && isRoad[indexUp]) {
            BfsRemove(i, j + 1, up[index], true);
          }
        }
      }
    }
  }

  void GetResult() {
    for (int i = 0; i < rowSize; i++) {
      for (int j = 0; j < columnSize; j++) {
        const size_t index = i * columnSize + j;
        const size_t indexLeft = (i - 1) * columnSize + j;
        const size_t indexRight = (i + 1) * columnSize + j;
        const size_t indexDown = i * columnSize + j - 1;
        const size_t indexUp = i * columnSize + j + 1;

        if (source[index] == 0 && sink[index] == 0 && isRoad[index] &&
            !isCut[index]) {
          if (result[index]) {
            if ((i != 0 && !result[indexLeft] && isRoad[indexLeft] &&
                 right[indexLeft] > 0) ||
                (i != rowSize - 1 && !result[indexRight] && isRoad[indexRight] &&
                 left[indexRight] > 0) ||
                (j != 0 && !result[indexDown] && isRoad[indexDown] &&
                 up[indexDown] > 0) ||
                (j != columnSize - 1 && !result[indexUp] && isRoad[indexUp] &&
                 down[indexUp] > 0)) {
              isCut[index] = true;
              // active = true;
            }
          } else {
            if ((i != 0 && result[indexLeft] && isRoad[indexLeft] &&
                 left[index] > 0) ||
                (i != rowSize - 1 && result[indexRight] && isRoad[indexRight] &&
                 right[index] > 0) ||
                (j != 0 && result[indexDown] && isRoad[indexDown] &&
                 down[index] > 0) ||
                (j != columnSize - 1 && result[indexUp] && isRoad[indexUp] &&
                 up[index] > 0)) {
              isCut[index] = true;
              // active = true;
            }
          }
        }
      }
    }
  }

 public:
  void SetData(GridGraph *gridGraph) {
    this->columnSize = gridGraph->GetColumnSize();
    this->rowSize = gridGraph->GetRowSize();

    source.resize(columnSize * rowSize, 0);
    sink.resize(columnSize * rowSize, 0);
    left.resize(columnSize * rowSize, 0);
    right.resize(columnSize * rowSize, 0);
    down.resize(columnSize * rowSize, 0);
    up.resize(columnSize * rowSize, 0);
    result.resize(columnSize * rowSize, false);
    isRoad.resize(columnSize * rowSize, false);
    isCut.resize(columnSize * rowSize, false);
    //prevIsCut.resize(columnSize * rowSize, false);

    this->grid = new Grid(columnSize, rowSize, 4, 32);

    for (int i = 0; i < rowSize; i++) {
      for (int j = 0; j < columnSize; j++) {
        const size_t index = i * columnSize + j;

        source[index] = gridGraph->GetSourceCapacity(i, j);
        sink[index] = gridGraph->GetSinkCapacity(i, j);
        left[index] = gridGraph->GetLeftCapacity(i, j);
        right[index] = gridGraph->GetRightCapacity(i, j);
        up[index] = gridGraph->GetUpCapacity(i, j);
        down[index] = gridGraph->GetDownCapacity(i, j);

        isRoad[index] =
            !(source[index] == 0 && sink[index] == 0 && left[index] == 0 &&
              right[index] == 0 && up[index] == 0 && down[index] == 0);
      }
    }
  }

  void GetCuts() {
    this->grid->set_caps(source.data(), sink.data(), down.data(), up.data(), left.data(), right.data());
    this->grid->compute_maxflow();
    this->WriteBack();
    this->GetResult();
    this->Split();
  }

  bool IsCut(int x, int y) {
    const size_t index = x * columnSize + y;
    return this->isCut[index];
    // bool r = this->isCut[index];
    // if (this->prevIsCut[index]) {
      // return false;
    // } else {
      // this->prevIsCut[index] = r;
      // return r;
    // }
  }
};
