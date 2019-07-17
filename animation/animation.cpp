#include "animation/animation.h"

Animation::Animation(meow_time_t start_time, meow_time_t end_time, InterpolatorType type) {
    startTime = start_time;

    if (end_time != 0 && end_time < startTime ) {
        endTime = startTime + end_time;
    } else {
        endTime = end_time;
    }

    interpolator = createInterpolator(type);
    parent = NULL;
}

Animation::Animation(Animation * animParent, meow_time_t duration, InterpolatorType type) {
    startTime = animParent->end();

    if (duration != 0) {
        ALOGE("DURATION ADDITIVE: %llu",duration);
        endTime = startTime + duration;
    } else {
        endTime = duration;
    }

    interpolator = createInterpolator(type);

    animParent->addChildAnimation(this);
    parent = animParent;
}

Animation::Animation(meow_time_t length) {
    startTime = currentTimeInMilliseconds();
    endTime = startTime + length;
    interpolator = createInterpolator(LINEAR_INTERPOLATOR);
}

Animation::~Animation() {
    delete interpolator;

    std::vector<Animation *>::iterator iter = childAnimations.begin();
    while (iter != childAnimations.end()) {
        delete *iter;
        iter++;
    }
}

bool Animation::drawAnimation(meow_time_t currentTime) {
    double interpolatedTimeScale = 0.0;
    bool shouldUseEnd = false;

    if (endTime > 0) {
        double interpolation = interpolator->interpolate(currentTime);
        interpolatedTimeScale = interpolation > 1.0 ? 1.0 : interpolation;
        shouldUseEnd = true;
    }

    if (currentTime < endTime || !shouldUseEnd) {
        animate(interpolatedTimeScale);
        return false;
    } else {
        bool done = true;
        for (int i = 0; i < childAnimations.size(); i++) {
            done = done && childAnimations[i]->drawAnimation(currentTime);
        }
        return done;
    }
}

Interpolator * Animation::createInterpolator(InterpolatorType interpolator) {
    switch (interpolator) {
        case QUADRATIC_INTERPOLATOR:
            return new QuadraticInterpolator(startTime,endTime);
        case LINEAR_INTERPOLATOR:
        default:
            return new LinearInterpolator(startTime,endTime);
    }
}

meow_time_t Animation::end() const {
    return endTime;
}
