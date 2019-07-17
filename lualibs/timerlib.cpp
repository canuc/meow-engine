#include <events/register_timer_event.h>
#include "lualibs/timerlib.h"
#include "lualibs/luaconstants.h"


static engine * eventEngine;

static int create_timer (lua_State *L,bool isSingleshot) {
    RegisterTimerEvent * timerEvent = new RegisterTimerEvent(isSingleshot);
    int numberOfParams = timerEvent->processEvent(L);
    postToDrawQueue(eventEngine,timerEvent);

    return numberOfParams;
}

static int create_recurring(lua_State *L) {
    return create_timer(L,false);
}

static int create_singleshot(lua_State *L) {
    return create_timer(L,true);
}

static int dolater(lua_State *L) {
    return create_timer(L,true);
}

static const luaL_Reg timerlib_functions[] = {
        {"create_timer",create_recurring},
        {"create_singleshot",create_singleshot},
        {"create_recurring",create_recurring},
        {"dolater",dolater},
        {NULL,NULL}
};

void load_timer_lib(lua_State * L,struct engine * engine) {
    const luaL_Reg *lib;
    eventEngine = engine;

    for (lib = timerlib_functions; lib->func; lib++) {
        lua_register(L,lib->name, lib->func);
    }
}

