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
#include "texture_loading.h"
#include "meow.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

GLuint texture_load(struct engine * gameEngine, const char * filename) {
    SDL_Surface *image;
    image=IMG_Load(filename);

    GLuint loadedTexture;
    if(!image) {
        printf("IMG_Load: %s\n", IMG_GetError());
        // handle error
    } else {
        glGenTextures(1, &loadedTexture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,loadedTexture);

        int textureMode = GL_RGB;

        if (image->format->BytesPerPixel == 4) {
            textureMode = GL_RGBA;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, textureMode, image->w, image->h, 0, textureMode, GL_UNSIGNED_BYTE, image->pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        SDL_FreeSurface(image);
    }
    return loadedTexture;
}

/*******************************************************
* This on SDL we do not need anything static out of the
* engine instance.
********************************************************/
void populate_asset_manager(engine * state) {
    return;
}

int32_t density_dpi(engine *state) {
    return 70;
}

#endif
