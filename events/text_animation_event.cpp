#include "events/text_animation_event.h"

void TextAnimationEvent::readEventData(lua_State* L)
{
    meow_time_t start, end;
    long long interpolator = LINEAR_INTERPOLATOR;
    Animation * parent = NULL;

    if (lua_isnumber(L, -3) )
    {
        start = lua_tointeger(L,-3);
    } else if (lua_istable(L, -3)) {
        parent = getAnimationFromLua(L,-3);
    }

    if (lua_isnumber(L, -2))
    {
        end = lua_tointeger(L,-2);
    }

    if (lua_isnumber(L, -1))
    {
        interpolator = lua_tointeger(L,-1);
    }

    if (!parent) {
        animation = new TextAnimation(fontManager, variableManager, start, end,
                                      (InterpolatorType) interpolator);
    } else {
        animation = new TextAnimation(fontManager, variableManager, parent, end,
                                      (InterpolatorType) interpolator);
    }

    if (lua_istable(L,-4))
    {
        // Pop the first two elements off the stack
        lua_pop(L,3);

        lua_pushnil(L);
        while(lua_next(L, -2) != 0)
        {
            const char *component = lua_tostring(L, -2);

            if (strncmp(component, ANIMATION_KEY_START, strlen(ANIMATION_KEY_START)) == 0 &&
                lua_istable(L, -1))
            {
                lua_getAnimationState(L, animation, true);
            }
            else if (strncmp(component, ANIMATION_KEY_END, strlen(ANIMATION_KEY_END)) == 0 &&
                     lua_istable(L, -1))
            {
                lua_getAnimationState(L, animation, false);
            }
            else if (strncmp(component, ANIMATION_KEY_FONT_NAME,
                             strlen(ANIMATION_KEY_FONT_NAME)) == 0 && lua_isstring(L, -1))
            {
                int fontId = fontManager->getFontFromName(lua_tostring(L, -1));

                if (fontId == -1)
                {
                    lua_pushstring(L,"Error the font was not found");
                    lua_error(L);
                }
                else
                {
                    animation->setFont(fontId);
                }
            }
            else if (strncmp(component, ANIMATION_KEY_TEXT, strlen(ANIMATION_KEY_TEXT)) == 0 &&
                     lua_isstring(L, -1))
            {
                animation->setText(lua_tostring(L,-1));
            }
            else if (strncmp(component, ANIMATION_KEY_SHADOW, strlen(ANIMATION_KEY_SHADOW)) == 0 &&
                     lua_istable(L, -1))
            {
                processShadow(L, animation);
            }
            else if (strncmp(component, ANIMATION_KEY_COLOR, strlen(ANIMATION_KEY_COLOR)) == 0 &&
                     lua_istable(L, -1))
            {
                animation->setColor(lua_getvec4(L));
            }

            lua_pop(L, 1);
        }
    }

    lua_pop(L,1);
    lua_pushinteger(L, getEventId());
}


void TextAnimationEvent::processShadow(lua_State * L, TextAnimation * animation)
{
    float opacity,offsetX,offsetY;

    lua_pushnil(L);
    while(lua_next(L, -2) != 0)
    {
        const char *component = lua_tostring(L, -2);

        if (strncmp(component, ANIMATION_KEY_SHADOW_OFFSET_X, strlen(ANIMATION_KEY_SHADOW_OFFSET_X)) == 0 &&
            lua_isnumber(L, -1))
        {
            offsetX = lua_tonumber(L,-1);
        }
        else if (strncmp(component, ANIMATION_KEY_SHADOW_OFFSET_Y, strlen(ANIMATION_KEY_SHADOW_OFFSET_Y)) == 0 &&
                 lua_isnumber(L, -1))
        {
            offsetY = lua_tonumber(L,-1);
        }
        else if (strncmp(component, ANIMATION_KEY_SHADOW_OPACITY, strlen(ANIMATION_KEY_SHADOW_OPACITY)) == 0 &&
                 lua_isnumber(L, -1))
        {
            opacity = lua_tonumber(L,-1);
        }

        lua_pop(L, 1);
    }

    animation->setShadow(opacity,offsetX,offsetY);
}

TextAnimation * TextAnimationEvent::takeAnimation() {
    TextAnimation * toTake = animation->build();
    animation = NULL;
    return toTake;
}