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
 */

#ifdef USE_SDL
#include "sdl_fopen.h"
#include <stdio.h>
#include <errno.h>
#include <SDL2/SDL.h>
#include "meow_util.h"
static ssize_t sdl_read(void *cookie, char *buf, size_t size) {
    SDL_RWops * asset = (SDL_RWops *) cookie;
    return asset->read(asset, buf, 1, (size_t) size);
}

static ssize_t sdl_write(void *cookie, const char *buf, size_t size) {
    SDL_RWops * asset = (SDL_RWops *) cookie;
    return asset->write(asset, buf, 1, (size_t) size);
}

static int sdl_seek(void* cookie, off_t * offset, int whence) {
    SDL_RWops * asset = (SDL_RWops *) cookie;
    off_t new_offset = asset->seek(asset, *offset, whence);
    *offset = new_offset;
    return 0;
}

static int sdl_close(void* cookie) {
    SDL_RWops * asset = (SDL_RWops *) cookie;
    return asset->close(asset);
}

FILE *_sdl_fopen(const char *fname, const char *mode) {
    SDL_RWops * rwOps = SDL_RWFromFile(getRelativeLocation("",fname).c_str(), mode);
    if(!rwOps) return NULL;
    cookie_io_functions_t  memfile_func = {
               .read  = sdl_read,
               .write = sdl_write,
               .seek  = sdl_seek,
               .close = sdl_close
    };
    return fopencookie(rwOps, mode, memfile_func);
}
#endif
