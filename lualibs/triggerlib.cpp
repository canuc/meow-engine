
#include "lualibs/triggerlib.h"
#include "lualibs/luaconstants.h"
#include <events/events.h>
#include <events/create_plane_trigger_event.h>
#include "work_queue.h"

static engine * eventEngine;

LUA_EVENT_FUNCTION(create_plane_trigger,new CreatePlaneTriggerEvent(eventEngine->workQueue),eventEngine)

static const luaL_Reg trigger_events[] = {
        {"create_plane_trigger",           create_plane_trigger},
        {0,0}
};

void load_trigger_lib(lua_State * L,struct engine * engine) {
    const luaL_Reg *lib;
    /* add open functions from 'preloadedlibs' into 'package.preload' table */
    eventEngine = engine;

    for (lib = trigger_events; lib->func; lib++) {
        lua_register(L,lib->name, lib->func);
    }
}
