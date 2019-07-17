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

#ifndef NYAN_ANDROID_TEXT_ANIMATION_EVENT_H
#define NYAN_ANDROID_TEXT_ANIMATION_EVENT_H

#include "drawable_event.h"
#include <string>
#include "work_queue.h"
#include "timer/lua_timer.h"
#include "animation/text_animation.h"
#include "events/lua_promise_event.h"
#include "event_ids.h"

#define X_KEY "x"
#define Y_KEY "y"
#define ALIGNMENT_TIME_KEY "alignment"

#define ANIMATION_KEY_START "start"
#define ANIMATION_KEY_END "finish"
#define ANIMATION_KEY_FONT_NAME "font"
#define ANIMATION_KEY_TEXT "text"
#define ANIMATION_KEY_COLOR "color"
#define ANIMATION_KEY_SHADOW "shadow"
#define ANIMATION_KEY_SHADOW_OFFSET_X "x"
#define ANIMATION_KEY_SHADOW_OFFSET_Y "y"
#define ANIMATION_KEY_SHADOW_OPACITY "opacity"

using namespace std;

class TextAnimationEvent: public LuaPromiseEvent
{
private:
    FontManager * fontManager;
    VariableManager * variableManager;
    TextAnimation * animation;
    int scopeRef;

    static void lua_getAnimationState(lua_State *L,TextAnimation * animation,bool start)
    {
        lua_pushnil(L);
        LUA_INTEGER x = 0,y = 0;
        LUA_INTEGER alignment_key = 0;

        while(lua_next(L, -2) != 0)
        {
            const char * component = lua_tostring(L, -2);
            if (strncmp(component, X_KEY, strlen(X_KEY)) == 0 && lua_isnumber(L, -1))
            {
                x = lua_tointeger(L,-1);
            }
            else if (strncmp(component, Y_KEY, strlen(Y_KEY)) == 0 && lua_isnumber(L, -1))
            {
                y = lua_tointeger(L,-1);
            }
            else if (strncmp(component, ALIGNMENT_TIME_KEY, strlen(ALIGNMENT_TIME_KEY)) == 0 && lua_isnumber(L, -1))
            {
                alignment_key = lua_tointeger(L,-1);
            }

            lua_pop(L, 1);
        }

        TextAnimation::TextWrapper wrapper =
        {
            .alignment=(TextAlignment) alignment_key,
            .offsetX=x,
            .offsetY=y
        };

        if (start)
        {
            animation->setRelativeStart(wrapper);
        }
        else
        {
            animation->setRelativeEnd(wrapper);
        }

        return;
    }

public:
    TextAnimationEvent(WorkQueue<LuaEvent> * promiseQueue,FontManager * font,VariableManager * vars):LuaPromiseEvent(promiseQueue,TEXT_ANIMATION_EVENT_ID,
                TEXT_ANIMATION_EVENT),fontManager(font),variableManager(vars), scopeRef(-1) { };
    ~TextAnimationEvent() { };

    virtual void readEventData(lua_State* L);

    void processShadow(lua_State * L, TextAnimation * animation);

    TextAnimation * takeAnimation();
};

#endif //NYAN_ANDROID_TEXT_ANIMATION_EVENT_H
