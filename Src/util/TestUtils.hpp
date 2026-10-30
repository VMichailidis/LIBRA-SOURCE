#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <math.h>
#include <stdlib.h>
#include <utility>

namespace utils {
double mean(double *values, int size) {
    // double sum{0.0};
    std::sort(values, values + size);
    return (values[size / 2] + values[size / 2 + 1]) / 2;
    // for (int i = 0; i < size; ++i)
    //     sum += values[i];
    // return (sum / size);
}

std::pair<int, int> get_indices(int size, float quantile) {
    float pos = size * quantile;

    std::pair<int, int> indices;

    // sets minimum index to 0 and adjusts index position
    if (pos < 1.0f) {
        pos = 0.0f;
    } else {
        pos -= 1.0f;
    }

    if (fabs(std::fmod(pos, 1.0)) < std::numeric_limits<float>::epsilon()) {
        // if pos is integer => use only single position
        indices.first = (int)pos;
        indices.second = (int)pos;
    } else {
        // use the mean between the two positions
        indices.first = floor(pos);
        indices.second = ceil(pos);
    }

    return indices;
}

std::pair<double, double> errors(double *values, int size, float quantile) {
    // sort values in-place
    std::sort(values, values + size);

    // determine indices of lower quantile
    std::pair<int, int> indices = get_indices(size, quantile);
    std::pair<double, double> errors;

    errors.first = (values[indices.first] + values[indices.second]) / 2.0;

    // now determine indices of upper quantile
    indices = get_indices(size, 1.0 - quantile);

    errors.second = (values[indices.first] + values[indices.second]) / 2.0;
    if (errors.second < errors.first)
        std::swap(errors.first, errors.second);

    return errors;
}
void myassert(double mean, std::pair<double, double> errors, double *times, size_t len) {
    if (!(mean >= errors.first * 0.99 && mean <= errors.second * 1.01)) {
        fprintf(stderr,
                "\n\033[31mERROR!\033[0m Mean (%f) does not reside within [%f, %f]\n",
                mean, errors.first, errors.second);
        printf("{");
        for (int i = 0; i < len; i++) {
            printf("%f,", times[i]);
        }
        printf("\b\b}");
        exit(-1);
    } else
        return;
}

} // namespace utils
