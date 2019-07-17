#include "gamethread.h"
#include "events/lua_event.h"
#include "work_queue.h"

#ifdef USE_ANDROID
#include "android_fopen.h"
#endif // USE_ANDROID
#include <pthread.h>

extern "C" {
#include "lauxlib.h"
#include "lualib.h"
#include "lapi.h"
}

#include "lua_util.h"
#include "lualibs/characterlib.h"
#include "lualibs/animationlib.h"
#include "lualibs/lua_promise.h"
#include "lualibs/bitlib.h"
#include "lualibs/gamelib.h"
#include "lualibs/timerlib.h"
#include "lualibs/triggerlib.h"
#include "lualibs/variablelib.h"

static uint8_t thread_shutdown = 1;
static pthread_mutex_t luastate_mutex = PTHREAD_MUTEX_INITIALIZER;

bool getIsShutdown() {
    bool isShutdown = false;
    pthread_mutex_lock(&luastate_mutex);
    isShutdown = thread_shutdown;
    pthread_mutex_unlock(&luastate_mutex);
    return isShutdown;
}

static void setIsShutdown(uint8_t shutdown) {
    pthread_mutex_lock(&luastate_mutex);
    thread_shutdown = shutdown;
    pthread_mutex_unlock(&luastate_mutex);
    return;
}


void * lua_game_thread(void * p_engine) {
    setIsShutdown(0);
    engine * gameEngine = (engine *) p_engine;
    lua_State* L=luaL_newstate();
    luaL_openlibs(L);
    /**
     * Load all the lua libraries
     */
    load_character_lib(L, gameEngine);
    load_animation_lib(L, gameEngine);
    load_bit_lib(L, gameEngine);
    load_game_lib(L, gameEngine);
    load_timer_lib(L, gameEngine);
    load_trigger_lib(L, gameEngine);
    load_variable_lib(L, gameEngine);

    if (luaL_dofile(L, gameEngine->scriptLocation))
    {
        ALOGE("Lua error: %s\n", lua_tostring(L, -1));
    }

    /*
     * Pull new out of the game module's new.
     */
    lua_getfield(L, -1, "new");

    if (!lua_isfunction(L, -1)) {
        lua_close(L);
        ALOGE("ERROR could not instantiate the game script! You must create a constructor.");
        return NULL;
    }

    /*
     * Move the counter module to be the first argument of new.
     */
    lua_insert(L, -1);

    if (lua_pcall(L, 1, 2, 0) != 0) {
        ALOGE("Lua error: %s\n", lua_tostring(L, -1));
        lua_close(L);
        return NULL;
    }

    /*
     *
     * Remove the empty filler nil from the top of the stack. The
     * lua_pcall stated 2 return values but on success we only
     * get one so we have nil filler after.
     *
     */
    lua_pop(L, 1);

    if (lua_type(L, -1) != LUA_TTABLE) {
        ALOGE("Invalid type (%d) returned by new\n", lua_type(L, -1));
        lua_close(L);
        return NULL;
    }

    lua_insert(L,-1);
    lua_setglobal(L, NYAN_MODULE);

    getPromise(L);

    while(true) {
        LuaEvent * currentEvent = gameEngine->workQueue->getEventBlocking();
        currentEvent->runEvent(L);
        bool shouldShutdown = currentEvent->getShouldShutdown();

        delete currentEvent;
        if (shouldShutdown) {
            break;
        }
    }

    lua_close(L);

    Event * e = NULL;
    while ((e = gameEngine->workQueue->getEvent())) {
        ALOGE("RELEASING EVENT: %p",e);
        delete e;
    }

    delete gameEngine->workQueue;
    gameEngine->workQueue = NULL;
    setIsShutdown(1);
    return NULL;
}

void start_game_thread(engine * engine) {
    pthread_t game_thread;
    int pthread_creation_result = pthread_create(&game_thread, NULL, lua_game_thread,(void *) engine);
    ALOGE("pthread_creation_result: %d", pthread_creation_result);
    if(pthread_creation_result) {
        ALOGE("Error creating thread\n");
    }
}
