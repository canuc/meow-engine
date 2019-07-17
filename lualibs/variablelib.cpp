//
// Created by julian on 4/4/16.
//

#include "meow.h"
#include "lualibs/variablelib.h"
#include "lualibs/class_util.h"
#include "variable_manager.h"
#include "variable/string_variable.h"

static engine * eventEngine;

static int variable_new(lua_State *L) {
    if (!lua_isuserdata(L, -1)) {
        lua_pushliteral(L, "You must have animation userdata as the first element");
        lua_error(L);
        return 0;
    }

    int userdata_loc = lua_absindex(L, -1);
    int self_loc = lua_absindex(L, -2);

    int lua_variable_manager = newtable(L);
    lua_pushstring(L, USER_DATUM);
    lua_pushvalue(L, userdata_loc);
    lua_settable(L, -3);

    lua_pushvalue(L, self_loc);
    lua_setmetatable(L, lua_variable_manager);
    return 1;
}

static VariableManager * getVariableManagerFromLua(lua_State *L, int stackPosition = -1) {
    return (VariableManager *) getUserDatum(L,stackPosition);
}

static int variable_get(lua_State *L) {
    VariableManager * variableManager = getVariableManagerFromLua(L,-2);

    if (!lua_isstring(L, -1)) {
        lua_pushliteral(L, "You must provide a string variable name");
        lua_error(L);
        return 0;
    }

    if (variableManager) {
        string variable_result = variableManager->getVariable(lua_tostring(L,-1));
        lua_pushstring(L,variable_result.c_str());
        return 1;
    } else {
        return 0;
    }
}

static int variable_set(lua_State *L) {
    VariableManager * variableManager = getVariableManagerFromLua(L,-3);

    if (!lua_isstring(L, -2)) {
        lua_pushliteral(L, "You must provide a string variable name");
        lua_error(L);
        return 0;
    }

    const char * set_var = lua_tostring(L, -1);
    if (variableManager) {
        variableManager->addVariable(lua_tostring(L,-2), new StringVariable(set_var));
        return 0;
    } else {
        return 0;
    }
}

static const luaL_Reg variable_methods[] = {
        {"new",variable_new},
        {"get",variable_get},
        {"set",variable_set},
        {0,0}
};

static int register_variable_class(lua_State *L) {
    return register_class(L,variable_methods,VARIABLE_CLASS);
}

void load_variable_lib(lua_State * L,struct engine * engine) {
    const luaL_Reg *lib;
    eventEngine = engine;

    register_variable_class(L);

    // We are also going to create the instance
    create_instance(L,engine->drawableState->variableManager,VARIABLE_CLASS);
    lua_setglobal(L,VARIABLE_INST);
}