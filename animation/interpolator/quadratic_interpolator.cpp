#include "animation/interpolator/quadratic_interpolator.h"

double QuadraticInterpolator::normalize_time(meow_time_t current_time, meow_time_t max) {
    meow_time_t current_sq = CUBE(current_time);
    meow_time_t max_sq = CUBE(max);

    return ((double) current_sq / (double) max_sq);
}