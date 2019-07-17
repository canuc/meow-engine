#include "events/camera_following_event.h"

int CameraFollowingEvent::processEvent(lua_State* L) {
    lua_pushnil(L);

    while(lua_next(L, -2) != 0) {
        const char *eventType = lua_tostring(L, -2);

        if (strncmp(eventType, ANIMATION_OFFSET_KEY, strlen(ANIMATION_OFFSET_KEY)) == 0 && lua_istable(L, -1)) {
            cameraOffset = lua_getvec3(L);
        }

        lua_pop(L, 1);
    }

    lua_pushinteger(L, getEventId());
    return 1;
}

CameraState * CameraFollowingEvent::createAnimationState(Actor * character) const {
    return new FollowingState(character,cameraOffset);
}