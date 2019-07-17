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

#ifndef NYAN_ANDROID_ANIMATION_H
#define NYAN_ANDROID_ANIMATION_H
#include "clock.h"
#include "interpolator/interpolators.h"
#include "lualibs/lua_serializable.h"
#include "lualibs/animationlib.h"
#include <vector>

enum {
    LINEAR_INTERPOLATOR=0,
    QUADRATIC_INTERPOLATOR,
};

typedef int InterpolatorType;

class Animation: public ILuaSerializable {
public:
    Animation() {
        parent = NULL;
    }

    Animation(meow_time_t start_time,meow_time_t end_time,InterpolatorType type = LINEAR_INTERPOLATOR);

    Animation(Animation * animParent, meow_time_t duration, InterpolatorType type = LINEAR_INTERPOLATOR);

    Animation(meow_time_t length);

    virtual ~Animation();

    virtual bool drawAnimation(meow_time_t currentTime);

    void serialize(lua_State * L) {
        create_lua_animation(L,this);
    }

    /**
     * Add a child animation.
     */
    void addChildAnimation(Animation * anim) {
        childAnimations.push_back(anim);
    }

    Animation * getParent() const {
        return parent;
    }

protected:

    virtual void animate(double interpolatedAnimation)=0;

    Interpolator * createInterpolator(InterpolatorType interpolator);

    meow_time_t end() const;

private:
    meow_time_t startTime;
    meow_time_t endTime;
    Interpolator * interpolator;
    Animation * parent;
    std::vector<Animation *> childAnimations;
};
#endif //NYAN_ANDROID_ANIMATION_H
