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

#ifdef USE_ANDROID
#include "texture_loading.h"
#include <android/bitmap.h>

GLuint texture_load(struct engine * gameEngine, const char * filename) {
    android_app * state = gameEngine->app;
    JNIEnv *env = state->activity->env;
    JavaVM *vm = state->activity->vm;
    jobject pngmgr = state->activity->clazz;
    int    ret;
    GLuint loadedTexture = 0;
    GLenum textureMode;
    GLenum textureFormat;

    vm->AttachCurrentThread(&env, NULL);

    jclass cls = env->GetObjectClass(pngmgr);
    jmethodID mid = env->GetMethodID(cls, "open", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
    jstring jname = env->NewStringUTF(filename);
    jobject bitmapJObject = env->CallObjectMethod(pngmgr, mid, jname);
    env->DeleteLocalRef(jname);

    // Hold a refrence to the bitmapJObject
    env->NewLocalRef(bitmapJObject);

    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(env, bitmapJObject, &info);

    switch(info.format) {
        case ANDROID_BITMAP_FORMAT_RGBA_8888:
            textureMode = GL_RGBA;
            textureFormat = GL_UNSIGNED_BYTE;
            break;
        case ANDROID_BITMAP_FORMAT_RGB_565:
            textureMode = GL_RGB;
            textureFormat = GL_UNSIGNED_SHORT_5_6_5;
            break;
        case ANDROID_BITMAP_FORMAT_RGBA_4444:
            textureMode = GL_RGBA;
            textureFormat = GL_UNSIGNED_SHORT_4_4_4_4;
            break;
        case ANDROID_BITMAP_FORMAT_A_8:
        default:
            ALOGE("Cannot handle alpha only pixel data");
            goto CLEANUP_BITMAP;
    }

    uint8_t * bytes;
    if((ret = AndroidBitmap_lockPixels(env,bitmapJObject,(void **) &bytes))  < 0) {
        ALOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        goto CLEANUP_BITMAP;
    }

    glGenTextures(1, &loadedTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,loadedTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, textureMode, info.width, info.height, 0, textureMode, textureFormat, bytes);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if((ret = AndroidBitmap_unlockPixels(env,bitmapJObject))  < 0) {
        ALOGE("AndroidBitmap_unlockPixels() failed ! error=%d", ret);
    }

CLEANUP_BITMAP:
    mid = env->GetMethodID(cls, "close", "(Landroid/graphics/Bitmap;)V");
    env->CallVoidMethod(pngmgr, mid, bitmapJObject);
    env->DeleteLocalRef(bitmapJObject);

    vm->DetachCurrentThread();

    return loadedTexture;
}

void populate_asset_manager(engine * state) {
    JNIEnv *env = state->app->activity->env;
    JavaVM *vm = state->app->activity->vm;
    jobject activity = state->app->activity->clazz;

    vm->AttachCurrentThread(&env, NULL);

    jclass cls = env->GetObjectClass(activity);
    jmethodID assetManagerCall = env->GetMethodID(cls, "getAssetManager", "()Landroid/content/res/AssetManager;");

    jobject assetManagerObject = env->CallObjectMethod(activity, assetManagerCall);

    if ( assetManagerObject == NULL) {
        ALOGE("NULL ASSET MANAGER");
    }

    state->assetManager = AAssetManager_fromJava(env, assetManagerObject);

    vm->DetachCurrentThread();

    return;
}

int32_t density_dpi(engine *state) {
    AConfiguration* config = AConfiguration_new();
    AConfiguration_fromAssetManager(config, state->app->activity->assetManager);
    int32_t density = AConfiguration_getDensity(config);
    AConfiguration_delete(config);
    return density;
}


/**
 * This is for debugging texutre atlas.
 */
void createLuminanceBitmap(engine * state, int w, int h, GLubyte * data) {
    JNIEnv * env= state->app->activity->env;
    JavaVM *vm = state->app->activity->vm;
    vm->AttachCurrentThread(&env, NULL);
    jclass java_bitmap_class = (jclass)env->FindClass("android/graphics/Bitmap");
    jmethodID mid = env->GetStaticMethodID(java_bitmap_class, "createBitmap", "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");

    const char * config_name = "ARGB_8888";
    jstring j_config_name = env->NewStringUTF(config_name);
    jclass bcfg_class = env->FindClass("android/graphics/Bitmap$Config");
    jobject java_bitmap_config = env->CallStaticObjectMethod(bcfg_class, env->GetStaticMethodID(bcfg_class, "valueOf", "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;"), j_config_name);
    jobject java_bitmap = env->CallStaticObjectMethod(java_bitmap_class, mid, w, h, java_bitmap_config);
    int    ret;
    uint8_t * bytes;
    if((ret = AndroidBitmap_lockPixels(env,java_bitmap,(void **) &bytes))  < 0) {
        ALOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    memcpy(bytes,data, w*h*sizeof(GLubyte)*4);

    if((ret = AndroidBitmap_unlockPixels(env,java_bitmap))  < 0) {
        ALOGE("AndroidBitmap_unlockPixels() failed ! error=%d", ret);
    }

    jobject pngmgr = state->app->activity->clazz;
    jclass cls = env->GetObjectClass(pngmgr);
    jmethodID bitmapWrite = env->GetMethodID(cls, "writeBitmap", "(Landroid/graphics/Bitmap;)V");
    env->CallVoidMethod(pngmgr, bitmapWrite, java_bitmap);

    vm->DetachCurrentThread();

    return;
}
#endif