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

#include "android_game.h"

char * AndroidGame::getScriptLocation(engine * gameEngine) {
    jobject pngmgr = gameEngine->app->activity->clazz;
    JavaVM *vm = gameEngine->app->activity->vm;
    JNIEnv *env = gameEngine->app->activity->env;


    vm->AttachCurrentThread(&env, NULL);

    jclass cls = env->GetObjectClass(pngmgr);
    jmethodID mid = env->GetMethodID(cls, "getLuaScript", "()Ljava/lang/String;");
    jstring scriptString = (jstring) env->CallObjectMethod(pngmgr, mid);

    const jsize len = env->GetStringUTFLength(scriptString);
    const char* strChars = env->GetStringUTFChars(scriptString, (jboolean *)0);

    char * scriptLocation = new char[len+1];
    memset(scriptLocation, 0x00, len+1);
    memcpy(scriptLocation, strChars, len);

    vm->DetachCurrentThread();
    return scriptLocation;
}

void AndroidGame::flush(engine * gameEngine) {
    Game::flush(gameEngine);

    eglSwapBuffers(gameEngine->display, gameEngine->surface);
}

void AndroidGame::frame(const glm::mat4 & model,const glm::mat4 & view, const glm::mat4 &perspective,const glm::vec3 & eyePos) {

    engine * gameEngine = getEngine();
    if (gameEngine->display == NULL) {
        // No display.
        return;
    }

    Game::frame(model, view, perspective, eyePos);
}

IUserControl * AndroidGame::getPlatformControl(engine * gameEngine){
    return new AndroidInput(gameEngine);
}
