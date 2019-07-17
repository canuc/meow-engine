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

#ifndef NYAN_ANDROID_EVENT_IDS_H
#define NYAN_ANDROID_EVENT_IDS_H

#define CREATE_FOLLOWING_EVENT_ID 140
#define CREATE_FOLLOWING_EVENT "camerafollowing"

#define CHARACTER_EVENT_ID 100
#define CHARACTER_EVENT "character"

#define DYNAMIC_CHARACTER_EVENT_ID 101
#define DYNAMIC_CHARACTER_EVENT "dynamic_character"

#define PRELOAD_CHARACTER_EVENT_ID 106
#define PRELOAD_CHARACTER_EVENT "preload_character"

#define DIED_EVENT_ID 3323
#define DIED_EVENT "characterDied"

#define REGISTER_TIMER_EVENT_ID 40
#define REGISTER_TIMER_EVENT "registerTimer"

#define SHUTDOWN_EVENT_ID 4
#define SHUTDOWN_EVENT "shutdown"

#define START_EVENT_ID 2
#define START_EVENT "gameLoaded"

#define TEXT_ANIMATION_EVENT_ID 81
#define TEXT_ANIMATION_EVENT "textAnimation"

#define TIMER_EVENT_ID 11
#define TIMER_FUNCTION_SENTINEL ""

#define UNLOCK_EVENT_ID 202
#define UNLOCK_EVENT "unlockevent"

#define UNLOCK_GRAVITY_EVENT_ID 203
#define UNLOCK_GRAVITY_EVENT "unlockgravityevent"

#define RESTART_EVENT_ID 204
#define RESTART_EVENT "restartevent"

#define WON_EVENT_ID 205
#define WON_EVENT "wonevent"

#define CHARACTER_CREATED_EVENT_ID 110
#define CHARACTER_CREATED_EVENT "characterCreated"

#define CREATE_ANIMATION_EVENT_ID 111
#define CREATE_ANIMATION_EVENT "cameraanimationevent"

#define CREATE_PLANE_TRIGGER_EVENT_ID 125
#define CREATE_PLANE_TRIGGER_EVENT "planetrigger"

#define FUNC_EVENT_ID 126
#define FUNC_EVENT "funcevent"

#define LUA_PROMISE_EVENT 36

#endif //NYAN_ANDROID_EVENT_IDS_H
