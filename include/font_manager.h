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

#ifndef NYAN_ANDROID_FONT_MANAGER_H
#define NYAN_ANDROID_FONT_MANAGER_H
#include "meow.h"
#include <map>
#include <ft2build.h>
#include <vector>
#include FT_FREETYPE_H
#include <lru_cache.h>

struct meow_font;
struct meow_text;

typedef lru_cache<string,meow_text> texts_t;
typedef map<string,meow_font *> fonts_t;

enum {
    TextAlignNone=0,
    TextAlignLeft=1,
    TextAlignRight=2,
    TextAlignCenterHorizontally=4,
    TextAlignTop=8,
    TextAlignBottom=16,
    TextAlignCenterVertically=32,
};

typedef uint16_t TextAlignment;

class FontManager {
private:
    FT_Library library;
    fonts_t fonts;
    ShaderManager * shaderManager;
    engine * gameEngine;
    int screenWidth;
    int screenHeight;
    texts_t texts;
    int textShader;
    int screenDensity;

    vector<meow_font *> weak_ref;
    std::map<string, int> font_names;
    meow_font * loadFontData(string filename,int32_t point_size,uint32_t DPI);
    meow_text * load_text(const char* msg,const meow_font* font) const;
    meow_text * get_text(const char * msg,const meow_font * font);

    void draw(meow_text * text,TextAlignment align = 0, glm::vec4 color = glm::vec4(1.0,1.0,1.0,1.0), int32_t offsetx = 0, int32_t offsety = 0);
    static string get_key(const char * msg, const meow_font * font);

    float getHorizontalPosition(TextAlignment alignmentFlags, const meow_text *text) const;
    float getVerticalPosition(TextAlignment alignmentFlags, const meow_text *text) const;

protected:
    meow_font * getFont(int32_t fontIndex);
    void drawText(const char * msg, meow_font * font, TextAlignment alignmentFlags = 0, glm::vec4 color = glm::vec4(1.0,1.0,1.0,1.0), int32_t offsetX =0, int32_t offsetY =0);
    meow_text * getText(const char * msg, meow_font * font);
    glm::mat4 getModelView(const meow_text * text,TextAlignment alignmentFlags, int32_t offsetX, int32_t offsetY) const;

public:
    FontManager(ShaderManager * shader, int screenWidth, int screenHeight, int density);
    ~FontManager();

    int32_t loadFont(string filename,int point_size);
    void drawText(const char * msg,int32_t fontId = 0);

    void drawText(const char *msg, TextAlignment alignmentFlags, glm::vec4 color, int32_t offsetX, int32_t offsetY,
                  int32_t fontId = 0);

    float getVerticalPosition(TextAlignment alignmentFlags, const char *string, int font);
    float getHorizontalPosition(TextAlignment alignmentFlags, const char *string, int font);

    int getFontFromName(string name);

    float dpiToPixels(int32_t dpi);

    float devicePixelRatio() const;
};

#endif //NYAN_ANDROID_FONT_MANAGER_H
