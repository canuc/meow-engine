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

#ifndef NYAN_ANDROID_TIMER_CPP_H
#define NYAN_ANDROID_TIMER_CPP_H

#include "meow.h"
#include <map>
#include <manager.h>
#include "irunnable.h"
#include "timer_types.h"

class Timer {
private:
    meow_time_t between;
    meow_time_t lastRun;
    bool singleShot;
    const timer_type_t timerType;

protected:
    virtual void run(meow_time_t currentTime,engine * gameEngine, bool expire)=0;

public:
    Timer(timer_type_t type, meow_time_t interval, bool isSingleShot = false):timerType(type) {
        this->between = interval;
        this->singleShot = isSingleShot;

        lastRun = currentTimeInMilliseconds();
    }

    virtual ~Timer() {}

    const timer_type_t getType() const {
        return timerType;
    }

    bool isSingleShot() const {
        return singleShot;
    }

    meow_time_t getBetween() const {
        return between;
    }

    bool check(meow_time_t currentTime,engine * engine) {
        bool hasRun = false;

        if ( lastRun + between < currentTime) {
            run(currentTime,engine,isSingleShot());
            lastRun = currentTime;
            hasRun = true;
        }

        return hasRun;
    }


};

typedef std::map<int32_t, Timer *> timer_map;

class TimerManager:public Manager<Timer>, public IRunnable {
public:
    TimerManager() {}
    virtual ~TimerManager() {}

    int32_t addTimer(int32_t timerId, Timer * timer);

    void run(meow_time_t currentTime,engine * gameEngine);
};


#endif //NYAN_ANDROID_TIMER_CPP_H
