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
#ifndef MEOW_LOGGING_H
#define MEOW_LOGGING_H

#define LOG_TAG "rightmeow"
#include <stdio.h>
#include <string.h>

#ifndef USE_ANDROID
#  define ERROR_LOG(args...) do { fprintf(stderr, args); fprintf(stderr,"\n"); fflush(stderr); } while(0)
#  define DEBUG_LOG(args...) do { fprintf(stderr, args); fprintf(stderr,"\n"); fflush(stderr); } while(0)
#else
#include <android/log.h>
#  define ERROR_LOG(...) do { (void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG,__VA_ARGS__); } while(0)
#  define DEBUG_LOG(...) do { (void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG,__VA_ARGS__); } while (0)
#endif

#include <stdlib.h>

void logger_standard(const char * log, int bufferSize);
void logger_error(const char * log, int bufferSize);
void format_error(const char * format, const char * str);


#define ALOGE(...) ERROR_LOG(__VA_ARGS__)
#define ALOGI(...) DEBUG_LOG(__VA_ARGS__)
#define LOGI(...) DEBUG_LOG(__VA_ARGS__)
#define LOGW(...) DEBUG_LOG(__VA_ARGS__)

#endif //MEOW_LOGGING_H
