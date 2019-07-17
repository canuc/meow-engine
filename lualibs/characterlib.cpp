#include <events/events.h>
#include "lualibs/characterlib.h"
#include <string.h>
#include "meow.h"
#include "work_queue.h"
#include "lua_util.h"
#include "lualibs/luaconstants.h"
#include "lualibs/class_util.h"
#include "cause_of_death.h"

#define CHARACTER_CLASS "CHARACTER"
static engine * eventEngine;

LUA_EVENT_FUNCTION(create_character,new CharacterEvent(eventEngine->workQueue),eventEngine)
LUA_EVENT_FUNCTION(create_dynamic_character,new DynamicCharacterEvent(eventEngine->workQueue),eventEngine)
LUA_EVENT_FUNCTION(preload_character, new PreloadCharacterEvent(eventEngine->workQueue),eventEngine)
LUA_EVENT_FUNCTION(create_follow,new CameraFollowingEvent(),eventEngine)

static int character_new(lua_State *L) {
    if (!lua_isuserdata(L, -1)) {
        lua_pushliteral(L, "You must have character userdata as the first element");
        lua_error(L);
        return 0;
    }

    if (!lua_istable(L, -2)) {
        lua_pushliteral(L, "First argument must be character");
        lua_error(L);
        return 0;
    }

    int userdata_loc = lua_absindex(L,-1);
    int self_loc = lua_absindex(L,-2);

    int character = newtable(L);
    lua_pushstring(L,USER_DATUM);
    lua_pushvalue(L,userdata_loc);
    lua_settable(L,-3);

    lua_pushvalue(L, self_loc);
    lua_setmetatable(L, character);
    return 1;
}

static Actor * getActorFromLua(lua_State *L, int stackPosition = -1) {
    return (Actor *) getUserDatum(L,stackPosition);
}

static int character_position(lua_State *L) {
    Actor * actor = getActorFromLua(L);

    if (actor) {
        createVec3Table(L,actor->getPosition());
        return 1;
    } else {
        return 0;
    }
}

static int character_mass(lua_State *L) {
    Actor * actor = getActorFromLua(L);

    if (actor) {
        lua_pushinteger(L,actor->getMass());
        return 1;
    } else {
        return 0;
    }
}

static int character_acceleration(lua_State *L) {
    Actor * actor = getActorFromLua(L);

    if (actor) {
        createVec3Table(L,actor->getVelocity());
        return 1;
    } else {
        return 0;
    }
}

static const luaL_Reg character_methods[] = {
        {"new",           character_new},
        {"position",      character_position},
        {"mass",          character_mass},
        {"character_acceleration", character_acceleration},
        {0,0}
};

static int register_character_class(lua_State *L) {
    return register_class(L,character_methods,CHARACTER_CLASS);
}

static const luaL_Reg loadedlibs[] = {
        {"create_character", create_character},
        {"create_dynamic_character", create_dynamic_character},
        {"create_follow",create_follow},
        {"preload_character", preload_character},
        {NULL,NULL}
};

static const lua_LConst characterLibConstants[] = {
        {"Collision", Collision},
        {"OutOfBounds",OutOfBounds},
        {NULL,NULL}
};

void create_lua_character(lua_State *L, Actor * actor) {
    create_instance(L, actor, CHARACTER_CLASS);
}

void load_character_lib(lua_State * L,struct engine * engine) {
    const luaL_Reg *lib;
    /* add open functions from 'preloadedlibs' into 'package.preload' table */
    eventEngine = engine;

    for (lib = loadedlibs; lib->func; lib++) {
        lua_register(L,lib->name, lib->func);
    }

    const lua_LConst *constants;
    for (constants = characterLibConstants; constants->name; constants++) {
        lua_pushinteger(L,constants->lua_int);
        lua_setglobal(L,constants->name);
    }

    register_character_class(L);
}
