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

#ifndef NYAN_ANDROID_CHARACTER_EVENT_H
#define NYAN_ANDROID_CHARACTER_EVENT_H



#include "lua_promise_event.h"
#include "glm/vec3.hpp"
#include <string>
#include "meow_util.h"
#include <map>
using namespace std;

#define POSITION_TABLE_KEY "position"
#define DESCRIPTOR_KEY "descriptor"
#define TEXTURE_KEY "texture"
#define SHADER_VERT_KEY "vert"
#define SHADER_FRAG_KEY "frag"
#define SCALE_TABLE_KEY "scale"
#define VELOCITY_KEY "velocity"
#define UNIFORMS_KEY "uniforms"

class CharacterEvent: public LuaPromiseEvent {
private:
    static const char * CHARACTER_EVENT_POSITION;
    static const char * CHARACTER_EVENT_DESCIPTOR;

    glm::vec3 position;
    glm::vec3 velocity;

    bool _hasPosition;
    bool _hasScale;
    bool _hasVelocity;

    float scale;

    string descriptor;
    string texture;
    string vert;
    string frag;
    map<string,float> uniforms;
protected:
    CharacterEvent(WorkQueue<LuaEvent> * promiseQueue, int eventId, const char * eventName);

    void setDescriptor(const string & descriptorFilename);

    void setTexture(const string & textureFilename);

public:
    CharacterEvent(WorkQueue<LuaEvent> * promiseQueue);

    ~CharacterEvent() { };

    void readEventData(lua_State* L);

    const string & getDescriptor() const;

    const string & getTexture() const;

    const string & getVert() const;

    const string & getFrag() const;

    bool hasPosition() const;

    bool hasScale() const;

    bool hasVelocity() const;

    float getScale() const;

    map<string,float> getUniforms() const;

    const glm::vec3 & getPosition() const;

    const glm::vec3 & getVelocity() const;

    virtual Actor * handleActorCreated(Actor * actor) const;

    virtual void addToScene(Actor * actor, engine * gameEngine) const;
};

#endif //NYAN_ANDROID_CHARACTER_EVENT_H
