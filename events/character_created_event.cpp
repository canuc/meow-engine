#include "events/character_created_event.h"
#include "events/event_ids.h"

CharacterCreatedEvent::CharacterCreatedEvent(const Actor * character):LuaEvent(CHARACTER_CREATED_EVENT_ID,
                                                        CHARACTER_CREATED_EVENT),createdCharacter(character) { };

CharacterCreatedEvent::~CharacterCreatedEvent() {

}

int CharacterCreatedEvent::populateEvent(lua_State *L) {
    lua_pushinteger(L, createdCharacter->getId());

    lua_createtable(L, 0, 1);

    lua_pushstring(L, CHARACTERMASS);
    lua_pushnumber(L, createdCharacter->getMass());
    lua_settable(L, -3);

    lua_pushstring(L, CHARACTERPOSITION);
    createVec3Table(L, createdCharacter->getPosition());
    lua_settable(L, -3);
    stackDump(L);
    return 2;
}