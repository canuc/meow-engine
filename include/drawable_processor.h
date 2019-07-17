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

#ifndef NYAN_ANDROID_DRAWABLEPROCESSOR_H
#define NYAN_ANDROID_DRAWABLEPROCESSOR_H

#include "meow.h"
#include "events/events.h"

class DrawableProcessor {
private:
    engine * gameEngine;
    void createCharacterFromEvent(const CharacterEvent *);

protected:
    void handleCharacter(const CharacterEvent *);
    void handleDynamicCharacter(const DynamicCharacterEvent *);
    void handlePreloadCharacter(const PreloadCharacterEvent *evnt);
    void handleCamera(const CameraAnimationEvent *);
    void handleTimerCreation(const RegisterTimerEvent *);
    void handleUnlockEvent(const UnlockEvent*);
    void handleUnlockGravityEvent(const UnlockGravityEvent *);
    void handleFollowingEvent(const CameraFollowingEvent *);
    void handleTextAnimationEvent(TextAnimationEvent *);
    void handleRestartEvent(const RestartEvent *);
    void handleCreateTriggerEvent(const CreatePlaneTriggerEvent *);
    void handleWonEvent(const GameWonEvent *);

public:
    DrawableProcessor(engine * engine);
    ~DrawableProcessor();

    void processDrawableEvent(DrawableEvent * event);


};


#endif //NYAN_ANDROID_DRAWABLEEVENT_H
