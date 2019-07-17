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

#ifndef NYAN_ANDROID_LUA_PROMISE_EVENT_H
#define NYAN_ANDROID_LUA_PROMISE_EVENT_H

#include "drawable_event.h"
#include "lualibs/lua_promise.h"

class LuaPromiseEvent: public DrawableEvent {
private:
    LuaPromise * promise;
    WorkQueue<LuaEvent> * queue;

public:
    LuaPromiseEvent(WorkQueue<LuaEvent> * promiseQueue,
                    const int32_t event_type,
                    const char * initializationType) : DrawableEvent(
            event_type, initializationType), queue(promiseQueue) { }


    virtual int processEvent(lua_State* L) {
        readEventData(L);
        LuaFunc success;
        LuaFunc error;
        newPromise(L,&success,&error);
        promise = new LuaPromise(queue,success,error);
        return 1;
    }

    virtual void readEventData(lua_State *L)=0;

    LuaPromise * getPromise() const {
        return promise;
    }
};
#endif //NYAN_ANDROID_LUA_PROMISE_EVENT_H
