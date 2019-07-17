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
#ifndef NYAN_ANDROID_MANAGER_H
#define NYAN_ANDROID_MANAGER_H

#include <stdint.h>
#include <map>
#include "logging.h"

using namespace std;

template<typename T,typename element_id = int32_t>
class Manager {
protected:
    typedef std::map<element_id, T *> ElementMap;
    typedef const ElementMap ConstElementMap;
    typedef typename ElementMap::iterator ElementIterator;
    typedef typename ElementMap::const_iterator ElementConstIterator;

    virtual void releaseElement(T * element) {
        return;
    }

    bool hasElement(element_id key) {
        return managedElements.find(key) != managedElements.end();
    }
private:
    ElementMap managedElements;

public:

    Manager() {
        clearAllElements();
    }

    virtual ~Manager() {
        ALOGE("Freeing Manager!");
        clearAllElements();
    }

    virtual int addElement(element_id elementId, T *element) {
        if (hasElement(elementId)) {
            T * oldElement = managedElements[elementId];
            delete oldElement;
        }

        managedElements[elementId] = element;
        return 0;
    }

    virtual T * getElement(element_id timerId) {
        ElementConstIterator iter = managedElements.find(timerId);

        if (iter != managedElements.end()) {
            return iter->second;
        }

        return NULL;
    }

    void clearAllElements() {
        ElementConstIterator iter = managedElements.begin();
        while (iter != managedElements.end()) {
            delete iter->second;
            iter++;
        }

        managedElements.clear();
    }

    void deleteElement(element_id elementId) {
        ElementIterator iter = managedElements.find(elementId);
        if (iter == managedElements.end()) {
            ALOGE("Error invalid element ID %d",elementId);
            return;
        }
        T * element = iter->second;
        releaseElement(element);
        managedElements.erase(iter);
        delete element;
    }

    ConstElementMap * elementMap() const {
        return &managedElements;
    }
};
#endif //NYAN_ANDROID_MANAGER_H
