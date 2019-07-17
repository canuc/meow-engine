#include "lualibs/lua_promise.h"

void getPromise(lua_State *L) {
    #ifdef DEBUG_LUA
        ALOGI("Initialize promise start TOP current: %d",lua_gettop(L));
    #endif

    lua_getglobal(L, NYAN_MODULE);
    lua_getfield(L, -1, NYAN_PROMISE);
    lua_setglobal(L, NYAN_PROMISE);

    lua_pop(L,1);

    #ifdef DEBUG_LUA
        ALOGI("Initialize promise library TOP current: %d",lua_gettop(L));
    #endif
}

void newPromise(lua_State *L, LuaFunc * success, LuaFunc * error) {
    static int newPromiseCount = 0;
    #ifdef DEBUG_LUA
        ALOGI("PROMISE: TOP current: %d call #: %d",lua_gettop(L), newPromiseCount);
        stackDump(L);
        ALOGI("calling event: %s\n", "Promise:new");
    #endif

    lua_getglobal(L, NYAN_DEFERRED);
    int errorStatus = lua_pcall(L, 0, 1, 0);
    if (errorStatus != 0) {
        ALOGE("Error calling new Promise(): %d\n",errorStatus);
        lua_pop(L,1);
    }

    int deferredScopeRef = luaL_ref(L, LUA_REGISTRYINDEX);

    lua_rawgeti(L, LUA_REGISTRYINDEX, deferredScopeRef);
    lua_getfield(L,-1,REJECT_METHOD);

#ifdef DEBUG_LUA
    ALOGI("Getting the promise reject field");
    stackDump(L);
#endif

    int rejectRef = luaL_ref(L,LUA_REGISTRYINDEX);
    lua_rawgeti(L, LUA_REGISTRYINDEX, deferredScopeRef);
    int rejectScope = luaL_ref(L,LUA_REGISTRYINDEX);

    lua_rawgeti(L, LUA_REGISTRYINDEX, deferredScopeRef);
    lua_getfield(L,-1,RESOLVE_METHOD);
#ifdef DEBUG_LUA
    ALOGI("Getting the promise resolve field");
    stackDump(L);
#endif

    int resolveRef = luaL_ref(L,LUA_REGISTRYINDEX);
    lua_rawgeti(L, LUA_REGISTRYINDEX, deferredScopeRef);
    int resolveScope = luaL_ref(L,LUA_REGISTRYINDEX);

    *success = LuaFunc(resolveRef,resolveScope, true);
    *error = LuaFunc(rejectRef,rejectScope,true);

    lua_pop(L,3);

    // get the deferred promise, and retrieve the function ref
    lua_rawgeti(L, LUA_REGISTRYINDEX, deferredScopeRef);
    lua_getfield(L, -1, NYAN_DEFERRED_PROMISE);
    lua_insert(L, -2);

    lua_pcall(L, 1, 1, 0);

    luaL_unref(L, LUA_REGISTRYINDEX, deferredScopeRef);

#ifdef DEBUG_LUA
    ALOGI("PROMISE: end top: %d call #: %d",lua_gettop(L),newPromiseCount);
    ALOGI("done calling event: %s\n", "Promise:new");
#endif

    newPromiseCount++;
}

LuaPromise::LuaPromise(WorkQueue<LuaEvent> * promiseQueue,
    const LuaFunc & success,
    const LuaFunc & reject): LuaEvent(LUA_PROMISE_EVENT, NULL), state(PENDING) {
        assert(promiseQueue);
        queue = promiseQueue;
        luaSuccess = success;
        luaReject = reject;
}

LuaPromise::LuaPromise(const LuaPromise & promise) : LuaEvent(LUA_PROMISE_EVENT,
                                                  NULL), state(PENDING)
{
    queue = promise.queue;
    luaSuccess = promise.luaSuccess;
    luaReject  = promise.luaReject;
}

void LuaPromise::postToQueue() {
    queue->addEvent(this);
}

void LuaPromise::resolve(ILuaSerializable * serializable) {
    if (state == PENDING) {
        this->serializable = serializable;
        this->state = RESOLVED;
        postToQueue();
    }
}

void LuaPromise::reject(ILuaSerializable * serializable) {
    if (state == PENDING) {
        this->serializable = serializable;
        this->state = REJECTED;
        postToQueue();
    }
}

bool LuaPromise::isSuccess() const {
    return state == RESOLVED;
}

void LuaPromise::runEvent(lua_State* L) {
    int numberOfParameters = 0;

#ifdef DEBUG_LUA
    ALOGI("TOP current: %d",startTop);
                ALOGI("calling event: %s\n", getFunctionName());
#endif
    LuaFunc * func = isSuccess() ? &luaSuccess : &luaReject;
    func->precall(L);

    if (serializable) {
        numberOfParameters++;
        serializable->serialize(L);
    }

    int errorStatus = func->call(L, numberOfParameters, 0, 0);
    if (errorStatus != 0) {
        ALOGE("Error calling timer event: %d:%d\n",func->getFunctionRefrence(),func->getScopeReference());
        stackDump(L);
        lua_pop(L,2);
        return;
    }

#ifdef DEBUG_LUA
    ALOGI("TOP new: %d",lua_gettop(L));
#endif

    luaSuccess.free(L);
    luaReject.free(L);
}

int LuaPromise::populateEvent(lua_State *L) {
    return 0;
}