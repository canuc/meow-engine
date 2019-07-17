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


#ifndef NYAN_ANDROID_LUA_UTIL_H
#define NYAN_ANDROID_LUA_UTIL_H
#include "logging.h"

extern "C" {
#include "lauxlib.h"
#include "lualib.h"
#include "lapi.h"
}

#include "glm/vec3.hpp"
#define newtable(L) (lua_newtable(L), lua_gettop(L))

/*!
 * \fn printLuaTable
 * \param L the lua program state
 *
 * Print out a table that is on the top of the lua stack.
 */
void printLuaTable(lua_State *L);

/*!
 * \fn stackDump
 * \param L the lua program state
 *
 * Print all the elements in the stack.
 */
void stackDump (lua_State *L);

/*!
 * \fn createVec3Table
 * \param L the lua program state
 * \param vec the vector to turn into a lua table
 *
 * Creates a new table with all the vector components.
 */
void createVec3Table(lua_State* L,const glm::vec3 & vec);

#endif //NYAN_ANDROID_LUA_UTIL_H
