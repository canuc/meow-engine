/*
 * Copyright 2019 Julian Haldenby
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software
 * is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef NYAN_ANDROID_LUA_CONSTANTS_H
#define NYAN_ANDROID_LUA_CONSTANTS_H

#include "work_queue.h"

typedef struct lua_LConst {
    const char *name;
    int lua_int;
} lua_LConst;

static bool postToDrawQueue(engine * gameEngine,DrawableEvent * event) {
    if (gameEngine->drawQueue) {
        gameEngine->drawQueue->addEvent(event);
    } else {
        ALOGE("Error with the drawable queue. Currently NULL.");
    }

    return true;
}

#define LUA_EVENT_FUNCTION(FUNCTION_CREATE_NAME,EVENT_CONSTRUCTOR,ENGINE) \
    static int FUNCTION_CREATE_NAME (lua_State *L) { \
        DrawableEvent * drawableEvent = EVENT_CONSTRUCTOR; \
        int numberOfParams = drawableEvent->processEvent(L); \
        postToDrawQueue(ENGINE,drawableEvent); \
        return numberOfParams; \
    }

#endif //NYAN_ANDROID_LUA_CONSTANTS_H
