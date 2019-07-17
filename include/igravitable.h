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

#ifndef IGRAVITABLE_H
#define IGRAVITABLE_H

#include <glm/vec3.hpp>
#include "inode.h"
#include "icollidable.h"

#define GRAVITATIONAL_CONSTANT 6.67384e-11

class IGravitable: public INode, public ICollidable {
    public:
        IGravitable() {};
        virtual ~IGravitable() {};

        virtual float getMass() const=0;
        virtual const glm::vec3 & getPosition() const=0;
        virtual glm::vec3 getScreenCoordinates() const;

        glm::vec3 calculateGravitationalForce(const IGravitable * gravity) const;
        void draw(meow_time_t time, ShaderManager * shaderManager,const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projectionView, const glm::vec3 & eyePos,const glm::vec3 & lookAt);
        bool operator<(const IGravitable &rhs) const;

    private:
        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 modelMat;
};

#endif // IGRAVITABLE_H
