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
#include "android_input.h"
#include "meow.h"
#include "actor.h"
#include "camera.h"
#define ANDROID_MAX_MOVE_ACCELERATION 0.001f
#define ANDROID_MAX_MOVE_DECELLERATION -0.0005f

int AndroidInput::addAndroidInput(AInputEvent *inputEvent) {
    int pointers = AMotionEvent_getPointerCount(inputEvent);

    if (AInputEvent_getType(inputEvent) == AINPUT_EVENT_TYPE_MOTION) {
        switch(pointers) {
            case 2:
                _gameEngine->drawableState->character->decelerate(0.001f);
                return 1;
            case 1:
            default:
                float clickx = AMotionEvent_getX(inputEvent, 0);
                float clicky = AMotionEvent_getY(inputEvent, 0);

                glm::vec2 click(1.0f * (clickx / _gameEngine->width * 2.0 - 1.0f),1.0f * (clicky / _gameEngine->height * 2.0 - 1.0));
                meow_time_t clickTime = (meow_time_t) (AMotionEvent_getEventTime(inputEvent));
                float timeDiff = 0.0;
                glm::vec2 diff;
                IInput * meowInputEvent;

                switch (AKeyEvent_getAction(inputEvent)) {
                    case AMOTION_EVENT_ACTION_MOVE:
                        timeDiff = (clickTime - _gameEngine->drawableState->mousedownTime)/10e9f;
                        diff = (click - _gameEngine->drawableState->mousedownLocation) * (timeDiff);
                        diff.x = glm::clamp(diff.x,ANDROID_MAX_MOVE_DECELLERATION,ANDROID_MAX_MOVE_ACCELERATION);
                        diff.y = glm::clamp(diff.y,ANDROID_MAX_MOVE_DECELLERATION,ANDROID_MAX_MOVE_ACCELERATION);

                        meowInputEvent = new IInput(INPUT_EVENT_MOVE,diff);
                        handleControl(_gameEngine->drawableState->character,meowInputEvent);
                        delete meowInputEvent;
                    case AMOTION_EVENT_ACTION_DOWN:
                        _gameEngine->drawableState->mousedownLocation = click;
                        _gameEngine->drawableState->mousedownTime = clickTime;

                        break;
                }


                return 1;
        }
    }

    return 0;
}

void AndroidInput::handleTilt(const ASensorEvent * event) {
    glm::vec3 tiltControl = _gameEngine->drawableState->camera->jitter();
    //tiltControl.x = glm::clamp((double)tiltControl.x + event->acceleration.x/100.0f,-1.0, 1.0);
    tiltControl.x = glm::clamp((double)tiltControl.x + (event->acceleration.y/-20.0f),-2.5,2.5);
    _gameEngine->drawableState->camera->setJitter(tiltControl);
}

void AndroidInput::pollInput(meow_time_t time) {



}