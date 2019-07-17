#include "events/camera_animation_event.h"

int CameraAnimationEvent::processEvent(lua_State* L) {
    lua_pushnil(L);

    while(lua_next(L, -2) != 0) {
        const char *eventType = lua_tostring(L, -2);

        if (strncmp(eventType, ANIMATION_START_KEY, strlen(ANIMATION_START_KEY)) == 0 && lua_istable(L, -1)) {
            animationStart = lua_getvec3(L);
        } else if (strncmp(eventType, ANIMATION_END_KEY, strlen(ANIMATION_END_KEY)) == 0 && lua_istable(L, -1)) {
            animationEnd = lua_getvec3(L);
        } else if (strncmp(eventType, ANIMATION_OFFSET_START_KEY, strlen(ANIMATION_OFFSET_START_KEY)) == 0 && lua_istable(L, -1)) {
            animationOffsetStart = lua_getvec3(L);
        } else if (strncmp(eventType, ANIMATION_OFFSET_END_KEY, strlen(ANIMATION_OFFSET_END_KEY)) == 0 && lua_istable(L, -1)) {
            animationOffsetEnd = lua_getvec3(L);
        } else if (strncmp(eventType, ANIMATION_OFFSET_DURATION_KEY, strlen(ANIMATION_OFFSET_DURATION_KEY)) == 0 && lua_isnumber(L, -1)) {
            animationDuration = (meow_time_t) lua_tonumber(L,-1);
        }

        lua_pop(L, 1);
    }

    lua_pushinteger(L, getEventId());
    return 1;
}

AnimationState * CameraAnimationEvent::createAnimationState() const {
    return new AnimationState(animationStart,animationEnd,animationOffsetStart,animationOffsetEnd,animationDuration);
}