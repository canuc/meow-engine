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

#ifndef WORK_QUEUE_H_INCLUDED
#define WORK_QUEUE_H_INCLUDED

#include <pthread.h>
#include <queue>
#include "logging.h"

using namespace std;
class Event;

template<typename T>
class WorkQueue {
private:
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    std::queue<T *> eventQueue;

public:
    WorkQueue() {
        cond = PTHREAD_COND_INITIALIZER;
        mutex = PTHREAD_MUTEX_INITIALIZER;
        int queueInitResult;
        queueInitResult = pthread_cond_init(&cond, NULL);
        if (queueInitResult != 0) {
            ALOGE("Error while initializing Wait condition!");
            return;
        }

        queueInitResult = pthread_mutex_init(&mutex, NULL);
        if (queueInitResult != 0) {
            ALOGE("Error while initializing mutex!");
            pthread_cond_destroy(&cond);
            return;
        }
    }

    ~WorkQueue() {
        pthread_cond_destroy(&cond);
        pthread_mutex_destroy(&mutex);
    }


    void addEvent(T * event) {
        pthread_mutex_lock(&mutex);
        size_t sizeBefore = eventQueue.size();
        eventQueue.push(event);

        if (sizeBefore == 0) {
            pthread_cond_signal(&cond);
        }

        pthread_mutex_unlock(&mutex);
    }

    void unpause() {
        pthread_mutex_lock(&mutex);
        size_t sizeBefore = eventQueue.size();

        if (sizeBefore == 0) {
            pthread_cond_signal(&cond);
        }

        pthread_mutex_unlock(&mutex);
    }


    T * getEventBlocking()  {
        pthread_mutex_lock(&mutex);

        if (eventQueue.size() == 0) {
            pthread_cond_wait(&cond, &mutex);
        }

        T * workQueueEvent  = eventQueue.front();
        eventQueue.pop();

        pthread_mutex_unlock(&mutex);

        return workQueueEvent;
    }

    T * getEvent() {
        T * workQueueEvent = NULL;

        pthread_mutex_lock(&mutex);

        if (eventQueue.size() > 0) {
            workQueueEvent = eventQueue.front();
            eventQueue.pop();
        }

        pthread_mutex_unlock(&mutex);

        return workQueueEvent;
    }


};

#endif // WORK_QUEUE_H_INCLUDED
