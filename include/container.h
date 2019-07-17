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
#ifndef CONTAINER_H
#define CONTAINER_H

#include <string>
using namespace std;
#include "meow.h"
#include "inode.h"
#include "igravitable.h"

#define NODE_TYPE_CONTAINER 2

class Container: public INode {
public:
    Container() {};
    virtual ~Container();

    glm::vec3 getForces(const IGravitable * gravitable) const;
    void drawNode(meow_time_t time, ShaderManager *shaderManager, const glm::mat4 &model,
                  const glm::mat4 &view, const glm::mat4 &projectionView, const glm::vec3 &eyePos,
                  const glm::vec3 &lookAt);
    ICollidable * checkCollision(const IGravitable *position);
    int getNodeType() const {
        return NODE_TYPE_CONTAINER;
    }

};

#endif
