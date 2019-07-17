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

#ifndef NYAN_ANDROID_LUA_PROMISE_H
#define NYAN_ANDROID_LUA_PROMISE_H

#include <events/lua_event.h>
#include <events/event_ids.h>
#include <work_queue.h>
#include "lualibs/lua_func.h"
#include "lua_globals.h"
#include "lua_serializable.h"
#include <assert.h>

#define RESOLVE_METHOD "resolve"
#define REJECT_METHOD "reject"

void getPromise(lua_State *L);

void newPromise(lua_State *L, LuaFunc * success, LuaFunc * error);

class LuaPromise: public LuaEvent {
private:
    typedef enum {
        PENDING,
        RESOLVED,
        REJECTED
    } t_promise_state;
    t_promise_state state;
    LuaFunc luaSuccess;
    LuaFunc luaReject;
    ILuaSerializable * serializable;
    WorkQueue<LuaEvent> * queue;

    void postToQueue();

public:
    LuaPromise(WorkQueue<LuaEvent> * promiseQueue,
               const LuaFunc & success,
               const LuaFunc & reject);

    LuaPromise(const LuaPromise & promise);

    void resolve(ILuaSerializable * serializable);

    void reject(ILuaSerializable * serializable);

    bool isSuccess() const;

    void runEvent(lua_State* L);

    int populateEvent(lua_State *L);
};

#endif //NYAN_ANDROID_LUA_PROMISE_H
