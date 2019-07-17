#include "lualibs/bitlib.h"

static engine * eventEngine;

static int or_op(lua_State *L) {
    int v = 0;
    lua_pushnil(L);

    while(lua_next(L, -2)) {
        v |= ((int)lua_tointeger(L, -1));
        lua_pop(L, 1);
    }

    lua_pushinteger(L,v);
    return 1;
}

static int and_op(lua_State *L) {
    int v = 0;
    lua_pushnil(L);

    while(lua_next(L, -2)) {
        v &= ((int)lua_tointeger(L, -1));
        lua_pop(L, 1);
    }

    lua_pushinteger(L,v);
    return 1;
}

static const luaL_Reg bitlib_functions[] = {
        {"or_op",or_op},
        {"and_op",and_op},
        {NULL,NULL}
};

void load_bit_lib(lua_State * L,struct engine * engine) {
    const luaL_Reg *lib;
    /* add open functions from 'preloadedlibs' into 'package.preload' table */
    eventEngine = engine;

    for (lib = bitlib_functions; lib->func; lib++) {
        lua_register(L,lib->name, lib->func);
    }

//    const lua_LConst *constants;
//    for (constants = bitlib_functions; constants->name; constants++) {
//        lua_pushinteger(L,constants->lua_int);
//        lua_setglobal(L,constants->name);
//    }
}

