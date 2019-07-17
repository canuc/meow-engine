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

#ifndef NYAN_ANDROID_ICOLLIDABLE_H_H
#define NYAN_ANDROID_ICOLLIDABLE_H_H

#include "ibounded.h"
#include "inamed.h"

typedef enum {
    COLLISION_MODEL_SPHERE=0,
    COLLISION_MODEL_BOUNDS,
    COLLISION_MODEL_PLANE
} t_collision_model;

#define SQR(a)		((a)*(a))
#define CUBE(a)		((a)*(a)*(a))

class ICollidable: public IBounded, public INamed {
protected:
    bool sphereWithBounds(const ICollidable * sphere,const ICollidable * box) const;
    bool sphereCollision(const ICollidable * sphere,const ICollidable * sphere1) const;
    bool boundsWithBounds(const ICollidable * box,const ICollidable * boxTwo) const;

    bool planeWithBounds(const ICollidable *plane, const ICollidable *box) const;
    bool planeWithSphere(const ICollidable * plane,const ICollidable *sphere) const;

    virtual bool collisionEnabled() const;

public:
    virtual bool collides(const ICollidable * collidable) const;

    virtual t_collision_model collisionModel() const=0;
    /*!
     * \fn collidedWith(ICollidable *)
     * \param collidable the other object in the collision
     * \brief the object that is collided with
     */
    virtual bool collidedWith(ICollidable * collidable);
};

#endif //NYAN_ANDROID_ICOLLIDABLE_H_H
