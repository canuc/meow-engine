//
// Created by julian on 2/7/16.
//

#include "events/character_event.h"
#include "scene_container.h"
#include "actor.h"
// Protected
CharacterEvent::CharacterEvent(WorkQueue<LuaEvent> * promiseQueue, int eventId, const char * eventName):
    LuaPromiseEvent(promiseQueue, eventId, eventName),
    _hasPosition(false),
    _hasScale(false),
    _hasVelocity(false),
    scale(1.0) {
};

CharacterEvent::CharacterEvent(WorkQueue<LuaEvent> * promiseQueue):
    LuaPromiseEvent(promiseQueue, CHARACTER_EVENT_ID, CHARACTER_EVENT),
    _hasPosition(false),
    _hasScale(false),
    _hasVelocity(false),
    scale(1.0) {

};

void CharacterEvent::setDescriptor(const string & descriptorFilename) {
    descriptor = getRelativeLocation("",descriptorFilename);
}

void CharacterEvent::setTexture(const string & textureFilename) {
    texture = getRelativeLocation("",textureFilename);
}

void CharacterEvent::readEventData(lua_State* L) {
    assert(L && lua_type(L,-1) == LUA_TTABLE );
    lua_pushnil(L);
    while(lua_next(L, -2) != 0) {
        const char *eventType = lua_tostring(L, -2);
        if (strncmp(eventType, POSITION_TABLE_KEY, strlen(POSITION_TABLE_KEY)) == 0 &&
            lua_istable(L, -1)) {
            position = lua_getvec3(L);
            _hasPosition = true;
        } else if (strncmp(eventType, DESCRIPTOR_KEY, strlen(DESCRIPTOR_KEY)) == 0 && lua_isstring(L,-1)) {
            string descriptorFilename(lua_tostring(L,-1));
            setDescriptor(descriptorFilename);
        } else if (strncmp(eventType, TEXTURE_KEY, strlen(TEXTURE_KEY)) == 0 && lua_isstring(L,-1)) {
            string texture(lua_tostring(L,-1));
            setTexture(texture);
        }  else if (strncmp(eventType, SCALE_TABLE_KEY, strlen(SCALE_TABLE_KEY)) == 0 && lua_isnumber(L,-1)) {
            _hasScale = true;
            scale = lua_tonumber(L,-1);
        } else if (strncmp(eventType, VELOCITY_KEY, strlen(VELOCITY_KEY)) == 0 && lua_istable(L, -1)) {
            _hasVelocity = true;
            velocity = lua_getvec3(L);
        } else if (strncmp(eventType, UNIFORMS_KEY, strlen(UNIFORMS_KEY)) == 0 && lua_istable(L, -1)) {
            lua_pushnil(L);

            while(lua_next(L, -2) != 0) {
                const char * component = lua_tostring(L, -2);
                if (lua_isnumber(L, -1)) {
                    uniforms[component] = (float) lua_tonumber(L,-1);
                }
                lua_pop(L, 1);
            }
        } else if (strncmp(eventType, SHADER_VERT_KEY, strlen(SHADER_VERT_KEY)) == 0 && lua_isstring(L,-1)) {
            vert = string(lua_tostring(L,-1));
        } else if (strncmp(eventType, SHADER_FRAG_KEY, strlen(SHADER_FRAG_KEY)) == 0 && lua_isstring(L,-1)) {
            frag = string(lua_tostring(L,-1));
        }

        lua_pop(L, 1);
    }
}

const string & CharacterEvent::getDescriptor() const {
    return descriptor;
}

bool CharacterEvent::hasPosition() const {
    return _hasPosition;
}

bool CharacterEvent::hasScale() const {
    return _hasScale;
}

bool CharacterEvent::hasVelocity() const {
    return _hasVelocity;
}

float CharacterEvent::getScale() const {
    return scale;
}

map<string,float> CharacterEvent::getUniforms() const {
    return uniforms;
};

const glm::vec3 & CharacterEvent::getPosition() const {
    return position;
}

const glm::vec3 & CharacterEvent::getVelocity() const {
    return velocity;
}

Actor * CharacterEvent::handleActorCreated(Actor * actor) const {
    return actor;
}

void CharacterEvent::addToScene(Actor *actor, engine *gameEngine) const {
    gameEngine->drawableState->screenContainer->addNode(actor);
}

const string & CharacterEvent::getTexture() const {
    return texture;
}

const string & CharacterEvent::getVert() const {
    return vert;
}

const string & CharacterEvent::getFrag() const {
    return frag;
}