#include "lualibs/class_util.h"

int register_class(lua_State *L,const luaL_Reg * classMethods,const char * className) {
    int methods;

    methods   = newtable(L);            /* Class methods */
    lua_pushvalue(L,methods);
    lua_pushliteral(L, "__index");      /* add index event to metatable */
    lua_pushvalue(L, methods);
    lua_settable(L, methods);         /* metatable.__index = methods */

    const luaL_Reg *lib;
    for (lib = classMethods; lib->func; lib++) {
        lua_pushstring(L, lib->name);
        lua_pushcfunction(L, lib->func);
        lua_settable(L, methods);
    }

    lua_setglobal(L,className);
    lua_pop(L,1);
    return 0;
}

void create_instance(lua_State *L, void * userData, const char * className) {
#ifdef DEBUG_LUA
    ALOGI("onEnter create_lua_character: ");
    stackDump(L);
    ALOGI("onEnter end create_lua_character");
#endif

    lua_getglobal(L,className);
    lua_getfield(L,-1,"new");
    lua_getglobal(L,className);
    lua_pushlightuserdata(L,userData);

    int errorStatus = lua_pcall(L, 2, 1, NULL);
    lua_insert(L,-2);
    lua_pop(L,1);

#ifdef DEBUG_LUA
    ALOGI("AFTER CREATING NEW CHARACTER: ");
    stackDump(L);
    ALOGI("AFTER DONE CREATING NEW CHARACTER!");
#endif

    if (errorStatus != 0) {
        ALOGE("Error calling Character:new(): %d  - %s\n",errorStatus, lua_tostring(L,-1));
        lua_pop(L,1);
    }

    return;
}

void * getUserDatum(lua_State *L, int stackPosition) {
    if (!lua_istable(L,stackPosition)) {
        ALOGE("With param should be table! %d",lua_type(L,stackPosition));
        lua_error(L);
        return NULL;
    }

    lua_getfield(L,stackPosition,USER_DATUM);

    if (!lua_isuserdata(L,-1)) {
        ALOGE("ERROR with userdata! %d",lua_type(L,-1));
        lua_error(L);
        return NULL;
    }

    void * datum = lua_touserdata(L,-1);
    lua_pop(L,1);

    return datum;
}