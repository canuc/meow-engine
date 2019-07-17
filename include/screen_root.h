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

#ifndef SCREENROOT_H
#define SCREENROOT_H

#include <vector>
#include <string>
using namespace std;
#include "meow.h"
#include "inode.h"

class ShaderManager;

class ScreenRoot: public INode {
public:
    ScreenRoot(ShaderManager * shaderManager, GLuint number = 400);
    ~ScreenRoot();
    void rotateScene(GLfloat x, GLfloat y);
    void setLine(glm::vec3 vecStart, glm::vec3 vecEnd);
    void setCamera(Camera * camera);
protected:
    void generatePoints(GLuint number);
    virtual void drawNode(meow_time_t time, ShaderManager *shaderManager, const glm::mat4 &model,
                  const glm::mat4 &view, const glm::mat4 &projectionView, const glm::vec3 &eyePos,
                  const glm::vec3 &lookAt);

    virtual glm::mat4 transformModelView(const glm::mat4 & modelView) const;
    void drawAfter(meow_time_t time, ShaderManager * shaderManager,const glm::mat4 & model,const glm::mat4 & view,const glm::mat4 & projectionView, const glm::vec3 & eyePos);
    void processNode(meow_time_t time, engine *gameEngine);
private:
    Camera * camera;
    GLuint vbo;
    GLuint indexBuffer;
    GLuint numberOfStars;
    GLint shader;

    GLfloat xDeg;
    GLfloat yDeg;

    GLboolean hasLine;
    GLuint lineBuffer;


};

#endif
