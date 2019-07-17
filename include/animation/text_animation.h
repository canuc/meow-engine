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

#ifndef NYAN_ANDROID_TEXT_ANIMATION_H_H
#define NYAN_ANDROID_TEXT_ANIMATION_H_H

#include <font_manager.h>
#include "animation.h"
#include <variable_manager.h>

class TextAnimation: public Animation
{
public:
    struct TextWrapper
    {
        TextAlignment alignment;
        LUA_INTEGER offsetX;
        LUA_INTEGER offsetY;
    };

    TextAnimation(FontManager * fontManager,VariableManager * vars,meow_time_t start_time,meow_time_t end_time,InterpolatorType type = LINEAR_INTERPOLATOR);

    TextAnimation(FontManager * fontManager,VariableManager * vars,Animation * parentAnimation,meow_time_t duration,InterpolatorType type = LINEAR_INTERPOLATOR);

    ~TextAnimation();

    TextAnimation * setText(const char * text);
    TextAnimation * setFont(int fontId);
    TextAnimation * setRelativeStart(TextAlignment alignment, int offsetX, int offsetY);
    TextAnimation * setRelativeEnd(TextAlignment alignment, int offsetX, int offsetY);
    TextAnimation * setRelativeStart(TextWrapper wrapper);
    TextAnimation * setRelativeEnd(TextWrapper wrapper);
    TextAnimation * setColor(glm::vec4 color);
    TextAnimation * setShadow(float opacity,float distX, float distY);

    TextAnimation * build();
private:
    FontManager * fontManager;
    VariableManager * variableManager;
    VariableCollection * collection;
    char * text;
    TextWrapper intializationWrapper[2];
    bool hasEnd;

    int fontId;
    float startX;
    float startY;
    float endX;
    float endY;
    glm::vec4 color;

    bool hasShadow;
    float shadowOffsetX;
    float shadowOffsetY;
    float shadowOpacity;
protected:
    virtual void animate(double interpolatedAnimation);

};


#endif //NYAN_ANDROID_TEXT_ANIMATION_H_H

