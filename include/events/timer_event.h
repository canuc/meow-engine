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

#ifndef NYAN_ANDROID_TIMEREVENT_H
#define NYAN_ANDROID_TIMEREVENT_H

#include <string>
#include "lua_event.h"
#include "lualibs/lua_func.h"
#include "event_ids.h"

using namespace std;

class TimerEvent: public LuaEvent {
private:
    LuaFunc function;

public:
    TimerEvent(LuaFunc & func):LuaEvent(TIMER_EVENT_ID, TIMER_FUNCTION_SENTINEL),function(func) {
    }

    void runEvent(lua_State* L) {
        int numberOfParameters = 0;

        #ifdef DEBUG_LUA
            ALOGI("TOP current: %d event: %p",startTop,this);
        #endif

        function.precall(L);

        int errorStatus = function.call(L, numberOfParameters, getNumberOfReturn(), 0);
        if (errorStatus != 0) {
            ALOGE("Error calling timer event: %d:%d\n",function.getFunctionRefrence(),function.getScopeReference());
            lua_pop(L,2);
            return;
        }

        //handleResult(L);

        function.free(L);
        #ifdef DEBUG_LUA
            ALOGI("TOP new: %d event: %p",lua_gettop(L),this);
        #endif
    }

    virtual int populateEvent(lua_State *L) {
        return 0;
    }
};

#endif //NYAN_ANDROID_TIMEREVENT_H
