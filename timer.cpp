#include "timer/timer.h"
#include <vector>
#include <timer/lua_timer.h>

int TimerManager::addTimer(int32_t timerId, Timer *timer) {
    return addElement(timerId,timer);
}

void TimerManager::run(meow_time_t currentTime,engine * gameEngine) {
    ConstElementMap * allAllocatedElements = elementMap();
    Manager<Timer>::ElementConstIterator iter = allAllocatedElements->begin();
    vector<int32_t> timersToDelete;

    while (iter != allAllocatedElements->end()) {
        if (iter->second->check(currentTime,gameEngine) ) {
            if (iter->second->isSingleShot()) {
                timersToDelete.push_back(iter->first);
            }
        }
        iter++;
    }

    vector<int32_t>::iterator timerToDelete = timersToDelete.begin();

    while (timerToDelete != timersToDelete.end()) {
        deleteElement(*timerToDelete);
        timerToDelete++;
    }

    return;
}
