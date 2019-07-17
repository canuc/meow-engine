#include "events/lua_event.h"

LuaEvent::LuaEvent(const int event_type,const char * func):
        Event(event_type),
        eventFunction(func),
        shouldShutdown(false) {

};

LuaEvent::~LuaEvent() {};

void LuaEvent::handleResult(lua_State *L) {
    lua_pop(L,getNumberOfReturn() || 1);
}

const char * LuaEvent::getFunctionName() const {
    return eventFunction;
}

void LuaEvent::runEvent(lua_State* L) {
#ifdef DEBUG_LUA
    ALOGI("TOP current: %d, event: %p",startTop,this);
            ALOGI("calling event: %s\n", getFunctionName());
#endif
    stackDump(L);
    int scopeParams = populateEventFunction(L);

#ifdef DEBUG_LUA
    ALOGI("before parameters current: %d, event: %p",lua_gettop(L),this);
        stackDump(L);
        ALOGI("before parameters end: %d, event: %p",lua_gettop(L),this);
#endif
    int numberOfParameters = populateEvent(L);
#ifdef DEBUG_LUA
    ALOGI("after populate TOP current: %d, event: %p",lua_gettop(L),this);
        stackDump(L);
        ALOGI("after populate TOP end: %d, event: %p",lua_gettop(L),this);
#endif
    ALOGI("dumping event: %s",eventFunction);
    stackDump(L);
    ALOGI("done dumping event: %s",eventFunction);
    int errorStatus = lua_pcall(L, numberOfParameters+scopeParams, getNumberOfReturn(), 0);
    if (errorStatus != 0) {
        ALOGE("Error calling event: %s:%d error: %s\n",eventFunction, errorStatus, lua_tostring(L, -1));
        lua_pop(L,2);
        return;
    }

    handleResult(L);
#ifdef DEBUG_LUA
    ALOGI("TOP new: %d from %p",lua_gettop(L),this);
#endif
}

int LuaEvent::populateEventFunction(lua_State* L) {
    lua_getglobal(L, NYAN_MODULE);
    lua_getfield(L, -1, getFunctionName());
    lua_getglobal(L, NYAN_MODULE);
    return 1;
}

int LuaEvent::getNumberOfReturn() {
    return 0;
}

bool LuaEvent::getShouldShutdown() const {
    return shouldShutdown;
}
