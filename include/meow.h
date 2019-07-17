#ifndef MEOW_H
#define MEOW_H

#include "meow_util.h"
#include <cstdio>
#include "logging.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#if USE_GLU
    #include <GL/glu.h>
#endif

#include "meow_gl.h"
#include <inttypes.h>
#include "glm/ext.hpp"

typedef enum {
    STATE_UNINITIALIZED,
    STATE_UNLOCKED,
    STATE_GRAVITY_ENABLED,
    STATE_GAME_WON,
    STATE_GAME_COLLISION,
    STATE_GAME_SHOULD_RESTART,
    STATE_GAME_RESTARTING
} t_game_state;

#define IS_GRAVITY_ENABLED(state) (((int) state) >= STATE_GRAVITY_ENABLED)
#define IS_UNLOCKED(state) (((int) state) >= STATE_UNLOCKED && ((int) state) <= STATE_GAME_SHOULD_RESTART)
#define IS_GAME_END(state) (((int) state) >= STATE_GAME_WON)
#define IS_GAME_WON(state) (((int) state) == STATE_GAME_WON)

#ifdef USE_ANDROID
    #include "meow_android.h"
#else
    #include "meow_linux.h"
#endif

#include "util/glutil.h"

#define LINE_BUFFER_SIZE 1024

#endif // MEOW_H
