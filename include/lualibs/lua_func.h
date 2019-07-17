/*
 * Copyright 2019 Julian Haldenby
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software
 * is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */
#ifndef NYAN_ANDROID_LUA_FUNC_H
#define NYAN_ANDROID_LUA_FUNC_H

#include "lua_util.h"

#define LUA_FUNC_NO_SCOPE -1
class LuaFunc {
private:
    int functionRef;
    int scopeReference;
    bool ownership;

public:
    LuaFunc(int functionRef, int scopeReference, bool ownership) {
        this->ownership = ownership;
        this->scopeReference = scopeReference;
        this->functionRef = functionRef;
    }

    LuaFunc(const LuaFunc & lua) {
        functionRef = lua.functionRef;
        scopeReference = lua.scopeReference;
        ownership = lua.ownership;
    }

    LuaFunc() {
        functionRef = LUA_FUNC_NO_SCOPE;
        scopeReference = LUA_FUNC_NO_SCOPE;
        ownership = false;
    }

    LuaFunc reference() const {
        return LuaFunc(functionRef, scopeReference, false);
    }

    int precall(lua_State *L) {
        int numberOfParameters = 0;
        lua_rawgeti(L, LUA_REGISTRYINDEX, functionRef);

        if (scopeReference != LUA_FUNC_NO_SCOPE) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, scopeReference);
            numberOfParameters = 1;
        }

        return numberOfParameters;
    }

    int call(lua_State *L, int numberOfParameters, int numberOfReturns, int errFunc) {
        if (scopeReference != LUA_FUNC_NO_SCOPE) {
            numberOfParameters++;
        }

        return lua_pcall(L, numberOfParameters, numberOfReturns, 0);
    }

    void free(lua_State * L) {
        if (ownership) {
            luaL_unref(L,LUA_REGISTRYINDEX,functionRef);
            luaL_unref(L,LUA_REGISTRYINDEX,scopeReference);
        }
    }

    int getFunctionRefrence() const {
        return functionRef;
    }

    int getScopeReference() const {
        return scopeReference;
    }
};

#endif //NYAN_ANDROID_LUA_FUNC_H
