#include "accelerator.h"

Accelerator::Accelerator(glm::vec3 * position):positionVector(position) {
    lastTime = 0;
}

void Accelerator::calcOffset(meow_time_t timeoffset) {
    if ( lastTime == 0 ) {
        lastTime = timeoffset;
        return;
    } else {
        meow_time_t difference = timeoffset - lastTime;
        // F = ma
        glm::vec3 accelerationDueToForce = this->mass == 0 ? glm::vec3() : this->forces / this->mass;
        accelerationDueToForce = glm::vec3(accelerationDueToForce.x,accelerationDueToForce.y,accelerationDueToForce.z);
        glm::vec3 translation = ((float) difference / (1000.0f)) * (velocity + accelerationDueToForce);
        positionVector->x = positionVector->x + translation.x;
        positionVector->y = positionVector->y + translation.y;
        //positionVector->z = positionVector->z + translation.z;
    }
}

void Accelerator::accelerate(const glm::vec3 & velocity) {
    this->velocity = this->velocity + velocity;
}

void Accelerator::calculateRateOfDeceleration(float decellerationMagnitude) {
    if ( decellerationMagnitude ) {
        glm::vec3 deceleration = glm::normalize(this->velocity) * -abs(decellerationMagnitude);
        if (decellerationMagnitude > glm::distance(glm::vec3(),this->velocity)) {
            this->velocity = glm::vec3();
        } else {
            this->velocity += deceleration;
        }
    }
}

void Accelerator::setForces(const glm::vec3 & forces) {
    this->forces = forces;
}

void Accelerator::setMass(float mass) {
    this->mass = mass;
}

const glm::vec3 &Accelerator::getVelocity() const {
    return this->velocity;
}
