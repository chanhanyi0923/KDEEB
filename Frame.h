#pragma once

#include <vector>

class Frame
{
public:
    std::vector<std::size_t> add;
    std::vector<std::size_t> remove;
    Frame();
    ~Frame();
};

