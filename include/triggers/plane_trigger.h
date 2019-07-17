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
#ifndef NYAN_ANDROID_PLANE_TRIGGER_H
#define NYAN_ANDROID_PLANE_TRIGGER_H

#include "lualibs/lua_func.h"
#include "inode.h"
#include "triggers/trigger.h"
#include "events/events.h"

class PlaneTrigger: public Trigger {
public:
    PlaneTrigger(WorkQueue<LuaEvent> * queue, glm::vec3 normal,float d,LuaFunc callback, int event):
            Trigger(COLLISION_MODEL_PLANE),
            bounds(normal,d),
            callbackFunc(callback),
            promiseQueue(queue),
            eventId(event),
            name("planeTrigger"){
    }

    BoundingPlane getBoundingPlane() const {
        return bounds;
    }

    bool collidedWith(ICollidable * collidable) {
        promiseQueue->addEvent(new FuncEvent(callbackFunc));

        return false;
    }


    void drawNode(meow_time_t time, ShaderManager *shaderManager, const glm::mat4 &model,
                  const glm::mat4 &view, const glm::mat4 &projectionView, const glm::vec3 &eyePos,
                  const glm::vec3 &lookAt) {
        // DONT DRAW!

    };

    int32_t getId() const {
        return eventId;
    }

    const string & getName() const {
        return name;
    }

private:
    BoundingPlane bounds;
    LuaFunc callbackFunc;
    WorkQueue<LuaEvent> * promiseQueue;
    int eventId;
    string name;
};

#endif