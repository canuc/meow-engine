#include "animation/text_animation.h"
#define INITIALIZATION_WRAPPER_START 0
#define INITIALIZATION_WRAPPER_END 1
#define MAX_TEXT_SIZE 100u
TextAnimation::TextAnimation(FontManager * fontManager,VariableManager * vars,meow_time_t start_time,meow_time_t end_time,InterpolatorType type):
        Animation(start_time,end_time,type),variableManager(vars),collection(NULL), hasEnd(false), hasShadow(false), shadowOffsetX(0), shadowOffsetY(0), shadowOpacity(0.0f) {
    this->fontManager = fontManager;
    this->text = NULL;
    this->color = glm::vec4(1.0,1.0,1.0,1.0);
    memset(intializationWrapper,0x00,sizeof(TextWrapper) * 2);
}

TextAnimation::TextAnimation(FontManager * fontManager, VariableManager * vars, Animation * parentAnimation,meow_time_t duration, InterpolatorType type):
Animation(parentAnimation, duration, type), variableManager(vars), collection(NULL), hasEnd(false), hasShadow(false), shadowOffsetX(0), shadowOffsetY(0), shadowOpacity(0.0f) {
    this->fontManager = fontManager;
    this->text = NULL;
    this->color = glm::vec4(1.0,1.0,1.0,1.0);
    memset(intializationWrapper, 0x00, sizeof(TextWrapper) * 2);
}

TextAnimation::~TextAnimation()
{
    if (this->text)
    {
        delete this->text;
    }
}


TextAnimation * TextAnimation::setText(const char * text)
{
    collection = variableManager->buildString(text);
    return this;
}

TextAnimation * TextAnimation::setFont(int fontId)
{
    this->fontId = fontId;
    return this;
}

TextAnimation * TextAnimation::setRelativeStart(TextAlignment alignment, int offsetX, int offsetY)
{
    TextWrapper wrapper = {alignment,offsetX,offsetY};
    return setRelativeStart(wrapper);
}

TextAnimation * TextAnimation::setRelativeEnd(TextAlignment alignment, int offsetX, int offsetY)
{
    TextWrapper wrapper = {alignment,offsetX,offsetY};
    return setRelativeEnd(wrapper);
}

TextAnimation * TextAnimation::build()
{
    string collectionString = collection->calculateString();
    TextWrapper * currentWrapper = &intializationWrapper[INITIALIZATION_WRAPPER_START];
    startX = fontManager->getHorizontalPosition(currentWrapper->alignment,collectionString.c_str(),this->fontId);
    startY  = fontManager->getVerticalPosition(currentWrapper->alignment,collectionString.c_str(),this->fontId);

    currentWrapper = &intializationWrapper[INITIALIZATION_WRAPPER_END];
    endX = fontManager->getHorizontalPosition(currentWrapper->alignment,collectionString.c_str(),this->fontId);
    endY  = fontManager->getVerticalPosition(currentWrapper->alignment,collectionString.c_str(),this->fontId);

    // Build complete!
    return this;
}

TextAnimation *TextAnimation::setRelativeStart(TextAnimation::TextWrapper wrapper)
{
    intializationWrapper[INITIALIZATION_WRAPPER_START] = wrapper;
    if(!hasEnd) {
        intializationWrapper[INITIALIZATION_WRAPPER_END] = wrapper;
    }
    return this;
}

TextAnimation *TextAnimation::setRelativeEnd(TextAnimation::TextWrapper wrapper)
{
    intializationWrapper[INITIALIZATION_WRAPPER_END] = wrapper;
    hasEnd = true;
    return this;
}

TextAnimation *TextAnimation::setColor(glm::vec4 color)
{
    this->color = color;

    return this;
}

void TextAnimation::animate(double interpolatedAnimation)
{
    int positionX = startX + ((endX - startX) * interpolatedAnimation);
    int positionY = startY + ((endY - startY) * interpolatedAnimation);
    string textString = collection->calculateString();

    if (hasShadow)
    {
        fontManager->drawText(textString.c_str(), TextAlignNone, glm::vec4(1.0, 1.0, 1.0, shadowOpacity), positionX + fontManager->dpiToPixels(shadowOffsetX),
                              positionY + fontManager->dpiToPixels(shadowOffsetY), fontId);
    }

    fontManager->drawText(textString.c_str(), TextAlignNone, color, positionX, positionY, fontId);
}

TextAnimation *TextAnimation::setShadow(float opacity, float distX, float distY)
{
    shadowOffsetX = distX;
    shadowOffsetY = distY;
    shadowOpacity = opacity;

    if ( distX != 0 || distY != 0)
    {
        hasShadow = true;
    }

    return this;
}
