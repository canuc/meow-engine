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

#ifndef IBOUNDED_H
#define IBOUNDED_H

#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <glm/gtx/normal.hpp>
#include <cstring>

void swapF(float * f1,float * f2);

struct BoundingRect {
    glm::vec3 top;
    glm::vec3 bottom;
    glm::vec3 normals[6];

    BoundingRect() {
    }

    BoundingRect(glm::vec3 top, glm::vec3 bottom) {
        this->top = top;
        this->bottom = bottom;

        updateNormals();
    }

    BoundingRect(const BoundingRect & bounds) {
        this->top = bounds.top;
        this->bottom = bounds.bottom;

        memcpy(&normals,&bounds.normals,sizeof(glm::vec3) * 6);
    }

    BoundingRect(const BoundingRect & bounds,const glm::mat3 & transformation) {
        this->top = transformation * bounds.top;
        this->bottom = transformation * bounds.bottom;

        updateNormals();
    }

    BoundingRect(const BoundingRect & bounds,const glm::mat4 & transformation) {
        glm::vec4 boundingTop = transformation * glm::vec4(bounds.top,1.0f);
        glm::vec4 boundingBottom = transformation * glm::vec4(bounds.bottom,1.0f);

        if ( boundingTop.x < boundingBottom.x ) {
            swapF(&boundingTop.x,&boundingBottom.x);
        }

        if ( boundingTop.y < boundingBottom.y ) {
            swapF(&boundingTop.y,&boundingBottom.y);
        }

        if ( boundingTop.z < boundingBottom.z ) {
            swapF(&boundingTop.z,&boundingBottom.z);
        }

        this->top = glm::vec3(boundingTop);
        this->bottom = glm::vec3(boundingBottom);

        updateNormals();
    }

    BoundingRect expand(float size) const {
        glm::vec3 expansion(size,size,size);
        return BoundingRect(top+expansion, bottom-expansion);
    }


    void updateNormals();
};

struct BoundingPlane {
public:
    glm::vec3 mNormal;
    float d;
    BoundingPlane(glm::vec3 normal,float planeD):mNormal(normal),d(planeD) {};
    BoundingPlane(const BoundingPlane & plane):mNormal(plane.mNormal),d(plane.d) {}
};

struct BoundingSphere {
    double radius;
    glm::vec3 center;

    BoundingSphere() {
    }

    BoundingSphere(glm::vec3 center,double radius) {
        this->center = center;
        this->radius = radius;
    }

    BoundingSphere operator*(const float scale) const {
        return BoundingSphere(center,radius*scale);
    }

    BoundingSphere operator+(const glm::vec3 position) const {
        return BoundingSphere(center + position,radius);
    }
};

class IBounded
{
    public:
        IBounded() {};
        virtual ~IBounded() {};
        virtual BoundingRect getBoundingRect() const=0;
        virtual BoundingSphere getBoundingSphere() const=0;
        virtual BoundingPlane getBoundingPlane() const {
            return BoundingPlane(glm::vec3(),0);
        }

        virtual bool isWithinBounds(const BoundingRect &) const;
    protected:
        BoundingSphere createBoundingSphere(const BoundingRect & rect) const {
            glm::vec3 center = (rect.top + rect.bottom) / 2.0f;
            return BoundingSphere(center,(rect.top.y - rect.bottom.y) / 2.0f);
        }

};

#endif // IBOUNDED_H
