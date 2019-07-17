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

#ifndef VARIABLE_MANAGER_H
#define VARIABLE_MANAGER_H

#include <variable/variable.h>
#include <vector>
#include "variable/variable_collection.h"
#include "variable/variable_wrapper.h"
#include "manager.h"

using namespace std;

class VariableManager: public Manager<VariableWrapper,string>
{
public:
    VariableManager();
    ~VariableManager();

    /*!
     * \fn addVariable(string variableName,Variable * access)
     * \brief Register a variable for use.
     * \param name The string to register the variable as
     * \param var The variable that is currently bound
     */
    int addVariable(const string & key, Variable *element);

    /*!
     * \fn buildString(const string & split)
     * \brief Build a set of variable spans so that substitution can be used.
     * \param split The string to split in to discrete chunks
     */
    VariableCollection * buildString(const string & split);

    string getVariable(const char * key);

    VariableWrapper * getElement(string id);
private:
    pthread_mutex_t mutex;
};
#endif
