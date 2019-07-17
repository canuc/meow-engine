#include "timer/lua_timer.h"
#include "work_queue.h"
#include "events/timer_event.h"

void LuaTimer::run(meow_time_t currentTime, engine * gameEngine, bool expire) {
    LuaFunc function = expire ? func : func.reference();
    gameEngine->workQueue->addEvent(new TimerEvent(function));
}

LuaTimer::LuaTimer(meow_time_t interval, const LuaFunc & functionRef, bool singleShot):Timer(TIMER_TYPE_LUA, interval,singleShot),func(functionRef) {
}