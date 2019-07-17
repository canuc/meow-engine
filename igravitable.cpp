#include "igravitable.h"
#include <glm/geometric.hpp>
#include <math.h>

glm::vec3 IGravitable::calculateGravitationalForce(const IGravitable * gravity) const {
    const glm::vec3 objectPosition = gravity->getPosition();
    const glm::vec3 position = getPosition();

    float objectMass = gravity->getMass();
    float mass = getMass();
    float distance = glm::distance(position, objectPosition);
    glm::vec3 objectPositionDifference = glm::normalize(position-objectPosition);
    float magnitude = (((double) objectMass * (double) mass) / (double) pow(unitToMeters(distance),2)) * GRAVITATIONAL_CONSTANT / 1000;
    return (objectPositionDifference) * magnitude;
}

glm::vec3 IGravitable::getScreenCoordinates() const {
    return glm::vec3(projection * view * modelMat * glm::vec4(getPosition(),1.0f));
}

void IGravitable::draw(meow_time_t time, ShaderManager * shaderManager,const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projectionView, const glm::vec3 & eyePos, const glm::vec3 & lookAt) {
    this->projection = projectionView;
    this->view = view;
    this->modelMat = model;
    INode::draw(time, shaderManager, model, view, projectionView, eyePos, lookAt);
}

bool IGravitable::operator<(const IGravitable &rhs) const {
    glm::vec3 coordinates = getScreenCoordinates();
    glm::vec3 rhsCoord = rhs.getScreenCoordinates();

    return coordinates.z < rhsCoord.z;
}
