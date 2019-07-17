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

#ifndef ANDROID_GAME_H_INCLUDED
#define ANDROID_GAME_H_INCLUDED

#include "game.h"
#include "meow.h"
#include "android_input.h"

class AndroidGame : public Game {
public:
    AndroidGame(engine * gameEngine): Game(gameEngine) {};
    ~AndroidGame() {};

protected:
    IUserControl * getPlatformControl(engine * gameEngine);

    void frame(const glm::mat4 & model,const glm::mat4 & view, const glm::mat4 &perspective,const glm::vec3 & eyePos);
    void flush(engine * gameEngine);
    char * getScriptLocation(engine * gameEngine);
};


#endif // ANDROID_GAME_H_INCLUDED
