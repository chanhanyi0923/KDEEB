#include "Steps.h"


Steps::Steps():
    size(0)
{
}


Steps::Steps(const size_t size):
    size(size)
{
    this->data.resize(size);

    double sum = 0;
    for (size_t i = 0; i < size; i ++) {
        double ytotal = 0;
        double cur_step = 0;
        for (size_t j = 0; j < size; j ++) {
            double i_y = std::atan(size - j);
            ytotal += i_y;
            if (i == j) {
                cur_step = i_y;
            }
        }
        double add = cur_step / ytotal;
        this->data[i] = std::make_pair(sum, add);
        sum += add;
    }
}


double Steps::GetAdd(const size_t index, const double curDis) const
{
    const double sum = this->data[index].first;
    const double add = this->data[index].second;

    return add * curDis / (1.0 - sum);
}


size_t Steps::GetSize() const
{
    return this->size;
}


