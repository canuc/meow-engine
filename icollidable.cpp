#include <icollidable.h>
#include "glm/ext.hpp"
#include "glm/vec3.hpp"

bool ICollidable::sphereWithBounds(const ICollidable * sphere,const ICollidable * box) const {
    BoundingSphere sphereBounds = sphere->getBoundingSphere();
    BoundingRect boxBounds = box->getBoundingRect();

    float * Bmin = glm::value_ptr(boxBounds.bottom);
    float * Bmax = glm::value_ptr(boxBounds.top);

    float * C = glm::value_ptr(sphereBounds.center);
    float r = (float) sphereBounds.radius;

    float  dmin;
    float  r2 = SQR( r );
    int    i, face;

    dmin = 0;
    for( i = 0; i < 3; i++ ) {
        if( C[i] < Bmin[i] ) dmin += SQR(C[i] - Bmin[i] ); else
        if( C[i] > Bmax[i] ) dmin += SQR( C[i] - Bmax[i] );
    }

    if( dmin <= r2 ) return true;
    return false;
}

bool ICollidable::sphereCollision(const ICollidable * firstSphere,const ICollidable * secondSphere) const {
    BoundingSphere sphereBounds = firstSphere->getBoundingSphere();
    BoundingSphere secondSphereBounds = secondSphere->getBoundingSphere();

    return glm::distance(sphereBounds.center,secondSphereBounds.center) <= (sphereBounds.radius + secondSphereBounds.radius);
}

bool ICollidable::boundsWithBounds(const ICollidable *box, const ICollidable *boxTwo) const {
    BoundingRect boxBounds = box->getBoundingRect();
    BoundingRect secondBounds = boxTwo->getBoundingRect();

    return(boxBounds.top.x > secondBounds.bottom.x &&
            boxBounds.bottom.x < secondBounds.top.x &&
            boxBounds.top.y > secondBounds.bottom.y &&
            boxBounds.bottom.y < secondBounds.top.y &&
            boxBounds.top.z > secondBounds.bottom.z &&
            boxBounds.bottom.z < secondBounds.top.z);
}

bool ICollidable::collides(const ICollidable *collidable) const {
    t_collision_model currentCollisionModel = collisionModel();
    t_collision_model otherCollisionModel = collidable->collisionModel();

    if (!collisionEnabled() || !collidable->collisionEnabled()) {
        return false;
    }

    switch(currentCollisionModel) {
        case COLLISION_MODEL_BOUNDS: {
            switch (otherCollisionModel) {
                case COLLISION_MODEL_BOUNDS:
                    return boundsWithBounds(this,collidable);
                case COLLISION_MODEL_SPHERE:
                    return sphereWithBounds(collidable,this);
                case COLLISION_MODEL_PLANE:
                    return planeWithBounds(collidable,this);
            }
            break;
        }
        case COLLISION_MODEL_PLANE: {
            switch (otherCollisionModel) {
                case COLLISION_MODEL_BOUNDS:
                    return planeWithBounds(this,collidable);
                case COLLISION_MODEL_SPHERE:
                    return planeWithSphere(this,collidable);
                case COLLISION_MODEL_PLANE:
                    return false;
            }
            break;
        }
        case COLLISION_MODEL_SPHERE: {
            switch (otherCollisionModel) {
                case COLLISION_MODEL_BOUNDS:
                    return sphereWithBounds(this,collidable);
                case COLLISION_MODEL_SPHERE:
                    return sphereCollision(this,collidable);
                case COLLISION_MODEL_PLANE:
                    return planeWithSphere(collidable,this);
            }

            break;
        }
        default: {
            return false;
        }
    }

    return false;
}

bool ICollidable::collisionEnabled() const {
    return false;
}

bool ICollidable::planeWithBounds(const ICollidable *plane, const ICollidable *box) const {
    BoundingPlane boundingPlane = plane->getBoundingPlane();
    BoundingRect boundingRect = box->getBoundingRect();
    glm::vec3 middle = (boundingRect.bottom + boundingRect.top) / 2.0f;

    // currently this is only axis aligned
    glm::vec3 verteces[] = {
            glm::vec3(boundingRect.top.x-middle.x, 0, 0 ),
            glm::vec3(0, boundingRect.top.y-middle.y, 0),
            glm::vec3(0, 0, boundingRect.top.z-middle.z)
    };

    double radius = glm::length(verteces[0]) * abs( glm::dot( glm::normalize(boundingPlane.mNormal), glm::normalize(verteces[0]) ) ) +
                  glm::length(verteces[1]) * abs( glm::dot( glm::normalize(boundingPlane.mNormal), glm::normalize(verteces[1]) ) ) +
                  glm::length(verteces[2]) * abs( glm::dot( glm::normalize(boundingPlane.mNormal), glm::normalize(verteces[2]) ) );

    double distance = glm::dot( boundingPlane.mNormal, middle ) - boundingPlane.d;

    return abs(distance) <= radius;
}

bool ICollidable::planeWithSphere(const ICollidable * plane,const ICollidable *sphere) const {
    BoundingPlane boundingPlane = plane->getBoundingPlane();
    BoundingSphere boundingSphere = sphere->getBoundingSphere();

    float separation = glm::dot(boundingSphere.center, boundingPlane.mNormal)
            - boundingPlane.d;

    return separation <= boundingSphere.radius;
}


bool ICollidable::collidedWith(ICollidable * collidable) {
    // NO-OP

    return true;
}