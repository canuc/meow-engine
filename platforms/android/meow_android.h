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

#ifndef PLATFORM_ANDROID_H
#define PLATFORM_ANDROID_H

#include <stdio.h>
#include <jni.h>
#include <errno.h>

#include <EGL/egl.h>
#include "android_native_app_glue.h"
#include <android/asset_manager_jni.h>
#include <android/sensor.h>
#include <events/drawable_event.h>
#include "glm/vec2.hpp"
#include "clock.h"
#include "user_control.h"
#include "android/android_std.h"

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
    glm::vec2 mousedownLocation;
    VariableManager * variableManager;
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
    struct android_app* app;

    ASensorManager* sensorManager;
    const ASensor* accelerometerSensor;
    ASensorEventQueue* sensorEventQueue;
    AAssetManager * assetManager;
    drawable_state * drawableState;
    WorkQueue<LuaEvent> * workQueue;
    WorkQueue<DrawableEvent> * drawQueue;
    Game * game;
    char * scriptLocation;


    int animating;
    t_game_state game_state;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;
    int32_t density;
    struct saved_state state;
};


#endif
