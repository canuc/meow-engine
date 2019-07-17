#include "lua_util.h"

void stackDump (lua_State *L) {
    int i;
    int top = lua_gettop(L);
    for (i = 1; i <= top; i++) {  /* repeat for each level */
        int t = lua_type(L, i);
        switch (t) {
            case LUA_TNIL:
                ALOGI("stack: %d = nil'",i);
                break;

            case LUA_TLIGHTUSERDATA:
                ALOGI("stack: %d = lightuserdata",i);
                break;

            case LUA_TTHREAD:
                ALOGI("stack: %d = thread'",i);
                break;

            case LUA_TUSERDATA:
                ALOGI("stack: %d = userdata'",i);
                break;

            case LUA_TSTRING:  /* strings */
                ALOGI("stack: %d = `%s'",i, lua_tostring(L, i));
                break;

            case LUA_TBOOLEAN:  /* booleans */
                ALOGI("stack: %d = %s",i,lua_toboolean(L, i) ? "true" : "false");
                break;

            case LUA_TNUMBER:  /* numbers */
                ALOGI("stack: %d = %g",i, lua_tonumber(L, i));
                break;

            default:  /* other values */
                ALOGI("stack: %d = %s",i, lua_typename(L, t));
                break;

        }
        printf("  ");  /* put a separator */
    }
    printf("\n");  /* end the listing */
}

void printLuaTable(lua_State *L) {
    lua_pushnil(L);

    while(lua_next(L, -2) != 0) {
        if (lua_isstring(L, -1)) {
            ALOGI("%s = %s\n", lua_tostring(L, -2), lua_tostring(L, -1));
        } else if (lua_isnumber(L, -1)) {
            ALOGI("%s = %d\n", lua_tostring(L, -2), lua_tonumber(L, -1));
        } else if (lua_isfunction(L, -1)) {
            ALOGI("%s = function\n", lua_tostring(L, -1));
        } else if(lua_istable(L, -1)) {
            ALOGI("%s = \n", lua_tostring(L, -2));
            printLuaTable(L);
        }

        lua_pop(L, 1);
    }
}

void createVec3Table(lua_State* L,const glm::vec3 & vec) {
    lua_createtable(L, 0, 3);
    int created_table = -3;
    lua_pushstring(L, "x");
    lua_pushnumber(L,vec.x);
    lua_settable(L, created_table);  /* 3rd element from the stack top */
    lua_pushstring(L, "y");
    lua_pushnumber(L, vec.y);
    lua_settable(L, created_table);  /* 3rd element from the stack top */
    lua_pushstring(L, "z");
    lua_pushnumber(L, vec.z);
    lua_settable(L, created_table);  /* 3rd element from the stack top */
}
