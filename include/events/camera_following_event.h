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

#ifndef NYAN_ANDROID_CAMERA_FOLLOWING_EVENT_H
#define NYAN_ANDROID_CAMERA_FOLLOWING_EVENT_H


#include "drawable_event.h"
#include "glm/vec3.hpp"
#include <string>
#include "meow_util.h"
#include "camera.h"
#include "main_character.h"
#include "event_ids.h"

using namespace std;

#define ANIMATION_OFFSET_KEY "offset"

class CameraFollowingEvent: public DrawableEvent {
private:
    glm::vec3 cameraOffset;

public:
    CameraFollowingEvent(): DrawableEvent(CREATE_FOLLOWING_EVENT_ID, CREATE_FOLLOWING_EVENT) { };

    ~CameraFollowingEvent() { };

    virtual int processEvent(lua_State* L);

    CameraState * createAnimationState(Actor * character) const;
};


#endif //NYAN_ANDROID_CAMERA_FOLLOWING_EVENT_H
