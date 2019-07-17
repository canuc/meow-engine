#include <events/events.h>
#include "lualibs/gamelib.h"
#include "lualibs/luaconstants.h"
static engine * eventEngine;

LUA_EVENT_FUNCTION(unlock_gravity,new UnlockGravityEvent(),eventEngine)
LUA_EVENT_FUNCTION(unlock_screen,new UnlockEvent(),eventEngine)
LUA_EVENT_FUNCTION(restart_game,new RestartEvent(),eventEngine)
LUA_EVENT_FUNCTION(won,new GameWonEvent(),eventEngine)

static int get_time(lua_State *L) {
    lua_pushinteger(L, (LUA_INTEGER) currentTimeInMilliseconds());
    return 1;
}

static const luaL_Reg gamelib_functions[] = {
        {"unlock_screen",unlock_screen},
        {"unlock_gravity",unlock_gravity},
        {"restart",restart_game},
        {"won",won},
        {"get_time",get_time},
        {NULL,NULL}
};

void load_game_lib(lua_State * L,struct engine * engine) {
    const luaL_Reg *lib;
    /* add open functions from 'preloadedlibs' into 'package.preload' table */
    eventEngine = engine;

    for (lib = gamelib_functions; lib->func; lib++) {
        lua_register(L,lib->name, lib->func);
    }

//    const lua_LConst *constants;
//    for (constants = gamelib_functions; constants->name; constants++) {
//        lua_pushinteger(L,constants->lua_int);
//        lua_setglobal(L,constants->name);
//    }
}
