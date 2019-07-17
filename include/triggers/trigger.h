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
#ifndef NYAN_ANDROID_TRIGGER_H
#define NYAN_ANDROID_TRIGGER_H

#include "inode.h"
#include "glm/vec3.hpp"
#include "icollidable.h"
#define NODE_TYPE_TRIGGER 3
class Trigger: public INode, public ICollidable {
public:
    Trigger(t_collision_model collisionModel):mCollisionModel(collisionModel) {

    }

    t_collision_model collisionModel() const {
        return mCollisionModel;
    }

    BoundingRect getBoundingRect() const {
        return BoundingRect();
    }

    BoundingSphere getBoundingSphere() const {
        return BoundingSphere();
    }

    BoundingPlane getBoundingPlane() const {
        return BoundingPlane(glm::vec3(),0);
    }

    bool collisionEnabled() const {
        return true;
    }

    int getNodeType() const {
        return NODE_TYPE_TRIGGER;
    }

private:
    t_collision_model mCollisionModel;
};

#endif