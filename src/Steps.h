#pragma once

#include <cstddef>
#include <vector>
#include <cmath>

class Steps
{
private:
    size_t size;

    // first: sum, second: add
    std::vector< std::pair<double, double> > data;

public:
    Steps();
    Steps(const size_t size);
    size_t GetSize() const;
    double GetAdd(const size_t index) const;
};

