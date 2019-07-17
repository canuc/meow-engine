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

#ifndef NYAN_ANDROID_DRAWABLE_EVENT_H
#define NYAN_ANDROID_DRAWABLE_EVENT_H

#include "event.h"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

extern "C" {
#include "lauxlib.h"
#include "lualib.h"
#include "lapi.h"
}
#include "lua_util.h"

#define POSITION_KEY_X "x"
#define POSITION_KEY_Y "y"
#define POSITION_KEY_Z "z"
#define POSITION_KEY_W "w"

#define POSITION_KEY_R "r"
#define POSITION_KEY_G "g"
#define POSITION_KEY_B "b"
#define POSITION_KEY_A "a"

class DrawableEvent : public Event {
private:
    const char * drawableType;
    static int globalEventId;
    const int32_t eventID;

    static const int getNextEventID() {
        return globalEventId++;
    }

public:
    DrawableEvent(const int32_t event_type,const char * initializationType): Event(event_type), drawableType(initializationType), eventID(getNextEventID()) { };
    ~DrawableEvent() { };

    /*!
     * \fn lua_getvec4
     * \param L lua vm state
     * \return glm::vec4 takes x,y,z,w and or r,g,b,a into vec4
     */
    static glm::vec4 lua_getvec4(lua_State *L);

    /*!
     * \fn lua_getvec3
     * \param L lua vm state
     * \return glm::vec3 takes x,y,z and or r,g,b into vec3
     */
    static glm::vec3 lua_getvec3(lua_State *L);

    const char * getDrawableType() const;
    virtual int processEvent(lua_State* L)=0;
    int32_t getEventId() const;
};

#endif //NYAN_ANDROID_DRAWABLE_EVENT_H
