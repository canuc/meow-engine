#include <events/events.h>
#include "lualibs/animationlib.h"
#include <string.h>
#include "lua_util.h"
#include "font_manager.h"
#include "lualibs/luaconstants.h"
#include "animation/animation.h"
#include "lualibs/class_util.h"


#define ANIMATION_CLASS "ANIMATION"
static engine * animationEngine;

static int animtion_new(lua_State *L) {
    if (!lua_isuserdata(L, -1)) {
        lua_pushliteral(L, "You must have animation userdata as the first element");
        lua_error(L);
        return 0;
    }

    if (!lua_istable(L, -2)) {
        lua_pushliteral(L, "First argument must be character");
        lua_error(L);
        return 0;
    }

    int userdata_loc = lua_absindex(L, -1);
    int self_loc = lua_absindex(L, -2);

    int animation = newtable(L);
    lua_pushstring(L, USER_DATUM);
    lua_pushvalue(L, userdata_loc);
    lua_settable(L, -3);

    lua_pushvalue(L, self_loc);
    lua_setmetatable(L, animation);
    return 1;
}

static int animation_one(lua_State *L){
    lua_pushinteger(L,1);
    return 1;
}

static const luaL_Reg animation_methods[] = {
        {"new",           animtion_new},
        {"one",           animation_one},
        {0,0}
};

static int register_animation_class(lua_State *L) {
    return register_class(L,animation_methods,ANIMATION_CLASS);
}

static const lua_LConst animationConstants[] = {
        // Text Alignments
        {"TextAlignNone", TextAlignNone},
        {"TextAlignLeft", TextAlignLeft},
        {"TextAlignRight", TextAlignRight},
        {"TextAlignCenterHorizontally", TextAlignCenterHorizontally},
        {"TextAlignTop", TextAlignTop},
        {"TextAlignBottom", TextAlignBottom},
        {"TextAlignCenterVertically", TextAlignCenterVertically},

        // Interpolators
        {"LINEAR_INTERPOLATOR", LINEAR_INTERPOLATOR},
        {"QUADRATIC_INTERPOLATOR", QUADRATIC_INTERPOLATOR},
        {NULL,NULL}
};

LUA_EVENT_FUNCTION(create_text_animation, new TextAnimationEvent(animationEngine->workQueue,animationEngine->drawableState->fontManager,animationEngine->drawableState->variableManager), animationEngine)
LUA_EVENT_FUNCTION(create_camera_animation, new CameraAnimationEvent(), animationEngine)


static const luaL_Reg animationLib[] = {
        {"create_camera_animation",create_camera_animation},
        {"text_animation",create_text_animation},
        {NULL,NULL}
};

void create_lua_animation(lua_State *L, Animation * actor) {
    create_instance(L, actor, ANIMATION_CLASS);
}

void load_animation_lib(lua_State * L, engine * engine) {
    const lua_LConst *constants;
    animationEngine = engine;

    for (constants = animationConstants; constants->name; constants++) {
        lua_pushinteger(L,constants->lua_int);
        lua_setglobal(L,constants->name);
    }

    const luaL_Reg *lib;
    for (lib = animationLib; lib->func; lib++) {
        lua_register(L,lib->name, lib->func);
    }

    register_animation_class(L);
}

Animation * getAnimationFromLua(lua_State *L, int stackPosition) {
    return (Animation *) getUserDatum(L,stackPosition);
}
