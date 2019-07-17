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

#ifndef UTIL_H
#define UTIL_H

#include "glm/matrix.hpp"
#include <string>
#include <meow.h>
#include "clock.h"

#define M_TO_UNITS 19113000.0

#define EOF_SENTINEL -1
using namespace std;

#ifdef USE_ANDROID
#include <android/asset_manager_jni.h>
/**
 * Android readline:
 *
 * We depend on assets in the asset directory
 */

char * readall(AAssetManager * assetManager,const string & filename);
#else
/**
* SDL readline
*
* We are going to use the SDL SDL_RWops
*/
#include <SDL2/SDL.h>
#endif

int readline(FILE * io, char * readbuffer);
void unquote( std::string& str );

void setRelativeBase(string relativeBase);
string getFilepath(const string & filepath);
string getFilename(const string & filepath);
string getRelativeLocation(const string & filedir,const string & filename);
double unitToMeters(double units);

#endif // UTIL_H
