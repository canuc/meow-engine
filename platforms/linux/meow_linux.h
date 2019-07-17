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

#ifdef USE_SDL
#ifndef PLATFORM_LINUX_H
#define PLATFORM_LINUX_H

#include <stdio.h>
#include <errno.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "glm/vec2.hpp"
#include "glm/mat4x4.hpp"
#include <events/drawable_event.h>
#include "clock.h"

class Actor;
class ScreenRoot;
class ModelManager;
class ShaderManager;
class LuaEvent;
class Event;
class Container;
class TextureManager;
class Camera;
class TimerManager;
class FontManager;
class IUserControl;
class AnimationManager;
class SceneContainer;
class Game;
class VariableManager;
template<typename T> class WorkQueue;

/**
 * Our drawable state.
 */
struct drawable_state {
    Actor * character;
    ScreenRoot* sceneGraph;
    SceneContainer * screenContainer;
    ShaderManager* shaderManager;
    ModelManager* modelManager;
    TextureManager* textureManager;
    TimerManager* timerManager;
    FontManager* fontManager;
    AnimationManager * animationManager;
    Camera* camera;
    IUserControl * userController;
    VariableManager * variableManager;
    glm::vec2 mousedownLocation;
    meow_time_t mousedownTime;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 perspective;

};

/**
 * Our saved state data.
 */
struct saved_state {
    GLuint state;
    glm::vec2 rotation;
    glm::vec4 selectedPosition;
    glm::vec4 selectedPositionEnd;
};

/**
 * Shared state for our app.
 */
struct engine {
    drawable_state * drawableState;
    WorkQueue<LuaEvent> * workQueue;
    WorkQueue<DrawableEvent> * drawQueue;
    Game * game;

    int animating;
    t_game_state game_state;

    int32_t width;
    int32_t height;
    int32_t density;
    struct saved_state state;
};
#endif
#endif
