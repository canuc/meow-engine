#include "events/drawable_event.h"

int DrawableEvent::globalEventId = 0;

glm::vec4 DrawableEvent::lua_getvec4(lua_State *L) {
    lua_pushnil(L);
    double x=0.0f,y=0.0f,z=0.0f,w=0.0f;
    while(lua_next(L, -2) != 0) {
        const char * component = lua_tostring(L, -2);
        if ((strncmp(component,POSITION_KEY_X, strlen(POSITION_KEY_X)) == 0 || strncmp(component,POSITION_KEY_R, strlen(POSITION_KEY_R)) == 0)
            && lua_isnumber(L, -1)) {
            x = lua_tonumber(L,-1);
        } else if ((strncmp(component,POSITION_KEY_Y, strlen(POSITION_KEY_Y)) == 0 || strncmp(component,POSITION_KEY_G, strlen(POSITION_KEY_G)) == 0)
                   && lua_isnumber(L, -1)) {
            y = lua_tonumber(L,-1);
        } else if ((strncmp(component,POSITION_KEY_Z, strlen(POSITION_KEY_Z)) == 0 || strncmp(component,POSITION_KEY_B, strlen(POSITION_KEY_B)) == 0)
                   && lua_isnumber(L, -1)) {
            z = lua_tonumber(L,-1);
        }  else if ((strncmp(component,POSITION_KEY_W, strlen(POSITION_KEY_W)) == 0 || strncmp(component,POSITION_KEY_A, strlen(POSITION_KEY_A)) == 0)
                    && lua_isnumber(L, -1)) {
            w = lua_tonumber(L,-1);
        }

        lua_pop(L, 1);
    }

    return glm::vec4(x,y,z,w);
}

glm::vec3 DrawableEvent::lua_getvec3(lua_State *L) {
    return glm::vec3(lua_getvec4(L));
}

const char * DrawableEvent::getDrawableType() const {
    return drawableType;
}

int32_t DrawableEvent::getEventId() const {
    return eventID;
}