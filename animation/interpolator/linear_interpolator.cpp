#include "animation/interpolator/linear_interpolator.h"

double LinearInterpolator::normalize_time(meow_time_t current_time, meow_time_t max) {
    return ((double) current_time / (double) max);
}