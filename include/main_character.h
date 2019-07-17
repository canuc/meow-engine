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

#ifndef MAINCHARACTER_H
#define MAINCHARACTER_H

using namespace std;

#include <string>
#include <vector>
#include "meow.h"
#include "actor.h"
#include "model_manager.h"
#include "shader_manager.h"
#include "variable/ivariable_binder.h"
#include "variable/function_variable.h"
#include "variable_manager.h"

class MainCharacter: public Actor, public IVariableBinder {
public:
    MainCharacter(const char * file, ModelManager * modelManager, ShaderManager * shader, TextureManager *textures): Actor(file,modelManager,shader,textures), mHealth(100.0f) {};
    ~MainCharacter() {};

    float getHealth() const {
        return mHealth;
    }

    void bindVars(VariableManager * var) const {
        var->addVariable("user.position.x", new FunctionVariable([this]() {
            return to_string(-getPosition().x);
        }));

        var->addVariable("user.position.y", new FunctionVariable([this]() {
            return to_string(getPosition().y);
        }));

        var->addVariable("user.position.z", new FunctionVariable([this]() {
            return to_string(getPosition().z);
        }));

        var->addVariable("user.velocity", new FunctionVariable([this]() {
            return to_string(glm::length(getVelocity()));
        }));

        var->addVariable("user.health", new FunctionVariable([this]() {
            return to_string(mHealth);
        }));
    }

protected:
    void drawNode(meow_time_t time, ShaderManager * shaderManager, const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projectionView, const glm::vec3 & eyePos, const glm::vec3 & lookAt);

private:
    double mHealth;
};

#endif
