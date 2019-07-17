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

#ifndef NYAN_ANDROID_USER_CONTROL_H
#define NYAN_ANDROID_USER_CONTROL_H

#include "glm/vec2.hpp"
#include <clock.h>

class Actor;

typedef enum {
    INPUT_EVENT_MOVE
} meow_input_event_t;


class IInput {
public:
    IInput(meow_input_event_t type, glm::vec2 inputVec) {
        setInput(type,inputVec);
    }

    IInput() {

    }

    void setInput(meow_input_event_t type, glm::vec2 inputVec) {
        vec = inputVec;
        inputEventType = type;
    }

    glm::vec2 inputVec() const {
        return vec;
    }

protected:
    glm::vec2 vec;
    int inputEventType;
};

class IUserControl {
public:
    virtual ~IUserControl() {};
    virtual void pollInput(meow_time_t time);

protected:
    virtual void handleControl(Actor * gravitable,const IInput * input);


};

#endif //NYAN_ANDROID_USER_CONTROL_H

