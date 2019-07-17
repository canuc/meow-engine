#ifndef NYAN_ANDROID_CREATE_TRIGGER_EVENT_H
#define NYAN_ANDROID_CREATE_TRIGGER_EVENT_H

#include "lua_promise_event.h"
#include "glm/vec3.hpp"
#include <string>
#include "meow_util.h"

using namespace std;

#define NORMAL_POSITION_KEY "normal"
#define PLANE_D_KEY "d"
#define CALLBACK_FUNCTION "callback"

class CreatePlaneTriggerEvent: public LuaPromiseEvent {
private:
    glm::vec3 normal;
    float d;
    LuaFunc callback;

public:
    CreatePlaneTriggerEvent(WorkQueue<LuaEvent> * promiseQueue);
    ~CreatePlaneTriggerEvent() { };

    void readEventData(lua_State* L);

    const LuaFunc & getCallback() const;

    float getPlaneD() const;

    glm::vec3 getNormal() const;
};

#endif //NYAN_ANDROID_CHARACTER_EVENT_H