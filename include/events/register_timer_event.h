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

#ifndef NYAN_ANDROID_TIMER_EVENT_H_H
#define NYAN_ANDROID_TIMER_EVENT_H_H

#include "drawable_event.h"
#include <string>
#include "work_queue.h"
#include "timer/lua_timer.h"
#include "event_ids.h"

using namespace std;

class RegisterTimerEvent : public DrawableEvent {
private:
    meow_time_t timeout;
    bool isSingleshot;
    LuaFunc func;

public:
    RegisterTimerEvent(bool singleshot = false): DrawableEvent(REGISTER_TIMER_EVENT_ID,
                                                               REGISTER_TIMER_EVENT),isSingleshot(singleshot) { };
    ~RegisterTimerEvent() { };

    virtual int processEvent(lua_State* L) {
        timeout = lua_tointeger(L,-1);

        int functionRef = -1;
        int scopeRef = -1;


        if (lua_isfunction(L,-2)) {
            lua_pushvalue(L, -2);
            functionRef = luaL_ref(L, LUA_REGISTRYINDEX);
        } else {

        }

        if (lua_istable(L,-3)) {
            lua_pushvalue(L, -3);
            scopeRef = luaL_ref(L, LUA_REGISTRYINDEX);
        }

        func = LuaFunc(functionRef,scopeRef,true);

        lua_pop(L,3);
        lua_pushinteger(L, getEventId());
        return 1;
    }

    Timer * createTimer() const {
        return new LuaTimer(timeout, func, isSingleshot);
    }
};

#endif //NYAN_ANDROID_TIMER_EVENT_H_H
