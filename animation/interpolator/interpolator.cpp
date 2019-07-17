#include "animation/interpolator/interpolator.h"
#include "util/glutil.h"

double Interpolator::interpolate(meow_time_t current_time) {
    meow_time_t total_diff = end_time - start_time;
    meow_time_t current_play_time = MEOW_MAX((meow_time_t) 0U, current_time - start_time);

    return normalize_time(current_play_time,total_diff);
}
