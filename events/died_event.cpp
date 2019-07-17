#include "events/died_event.h"

DiedEvent::DiedEvent(CauseOfDeath causeOfDeath):LuaEvent(DIED_EVENT_ID, DIED_EVENT),cause(causeOfDeath) {

};

int DiedEvent::populateEvent(lua_State *L) {
    lua_pushinteger(L,cause);
    return 1;
}