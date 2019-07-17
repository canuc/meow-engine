//
// Created by julian on 3/30/16.
//

#include "events/create_plane_trigger_event.h"

CreatePlaneTriggerEvent::CreatePlaneTriggerEvent(WorkQueue<LuaEvent> * promiseQueue):
LuaPromiseEvent(promiseQueue, CREATE_PLANE_TRIGGER_EVENT_ID, CREATE_PLANE_TRIGGER_EVENT) {
};

void CreatePlaneTriggerEvent::readEventData(lua_State* L) {
    assert(L && lua_type(L,-1) == LUA_TTABLE );
    lua_pushnil(L);
    while(lua_next(L, -2) != 0) {
        const char *eventType = lua_tostring(L, -2);
        if (strncmp(eventType, NORMAL_POSITION_KEY, strlen(NORMAL_POSITION_KEY)) == 0 &&
            lua_istable(L, -1)) {
            normal = lua_getvec3(L);
        } else if (strncmp(eventType, PLANE_D_KEY, strlen(PLANE_D_KEY)) == 0 && lua_isnumber(L,-1)) {
            d = lua_tointeger(L,-1);
        } else if (strncmp(eventType, CALLBACK_FUNCTION, strlen(CALLBACK_FUNCTION)) == 0 && lua_isfunction(L,-1)) {
            stackDump(L);
            int ref = luaL_ref(L, LUA_REGISTRYINDEX);
            callback = LuaFunc(ref, LUA_FUNC_NO_SCOPE, true);
            lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
            stackDump(L);
        }
        lua_pop(L, 1);
        stackDump(L);
    }
}

const LuaFunc & CreatePlaneTriggerEvent::getCallback() const {
    return callback;
}

float CreatePlaneTriggerEvent::getPlaneD() const {
    return d;
}

glm::vec3 CreatePlaneTriggerEvent::getNormal() const {
    return normal;
}