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

#ifndef NYAN_BASE_GAME_H
#define NYAN_BASE_GAME_H

#include <variable/ivariable_binder.h>
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "inttypes.h"
#include "user_control.h"
#include "meow_gl.h"

class Plugin;
struct engine;
class MD5Model;

class Game : public IVariableBinder{
public:
    Game(engine * gameEngine);
    virtual ~Game();
    /*!
     * Bootstrap game.
     */
    void bootstrap();

    void drawframe();
    /*!
     * Shutdown the game. Deallocate any resources.
     */
    // void shutdown();
    void restart();
    void bindVars(VariableManager * var) const;
protected:
    virtual IUserControl * getPlatformControl(engine * engine)=0;
    virtual void frame(const glm::mat4 & model,const glm::mat4 & view, const glm::mat4 &perspective,const glm::vec3 & eyePos);
    virtual void flush(engine * gameEngine);
    engine * getEngine() const {
        return gameEngine;
    }
    virtual char * getScriptLocation(engine * gameEngine)=0;
    void initializeHDR();

    void flushFBO(engine * gameEngine);

    void drawFullscreen(engine * gameEngine,int fullShaderId);
private:
    void initializeFreetype();
    void intializeQueues();
    void shutdown();

    uint32_t frames;
    float framesPerSecond;
    GLuint framebuffer;
    GLuint colorBuffers[2];
    GLuint attachments[2];
    GLuint screenVbo[3];
    GLuint renderBuffer;
    int shaderId;
    int blurShaderId;
    int md5ShaderId;
    GLuint pingpongFBO[2];
    GLuint pingpongColorbuffers[2];
    GLuint monkeyTexture;

    static const GLfloat square[4 * 2];
    static const GLfloat squareTextureCoords[4*2];
    static const GLushort indexes[6];

    engine * gameEngine;
    Actor * coconut;
};


#endif //NYAN_ANDROID_GAME_H
