//
// Created by julian on 4/3/16.
//
#include "events/func_event.h"
#include "events/event_ids.h"

FuncEvent::FuncEvent(LuaFunc callback):LuaEvent(FUNC_EVENT_ID, FUNC_EVENT),func(callback) { };
FuncEvent::~FuncEvent() {};

int FuncEvent::populateEventFunction(lua_State* L) {
    return func.precall(L);
}

int FuncEvent::populateEvent(lua_State *L) {
    return 0;
}