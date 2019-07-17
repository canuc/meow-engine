#include <sys/time.h>
#include "clock.h"

#ifdef USE_ANDROID
meow_time_t currentTimeInMilliseconds()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((((meow_time_t) tv.tv_sec) * 1000) + (((meow_time_t) tv.tv_usec) / (meow_time_t) 1000));
}
#else
#include <SDL2/SDL.h>
meow_time_t currentTimeInMilliseconds()
{
    return SDL_GetTicks();
}
#endif
