#include "ibounded.h"

void swapF(float * f1,float * f2) {
    float tmp = *f1;
    *f1 = *f2;
    *f2 = tmp;
}

void BoundingRect::updateNormals() {
    glm::vec3 bottomBackLeft = glm::vec3(
            bottom.x,
            bottom.y,
            bottom.z
    );

    glm::vec3 bottomBackRight = glm::vec3(
            top.x,
            bottom.y,
            bottom.z
    );

    glm::vec3 bottomFrontLeft = glm::vec3(
            bottom.x,
            bottom.y,
            top.z
    );

    glm::vec3 bottomFrontRight = glm::vec3(
            top.x,
            bottom.y,
            top.z
    );

    glm::vec3 topBackLeft = glm::vec3(
            bottom.x,
            top.y,
            bottom.z
    );
    glm::vec3 topBackRight = glm::vec3(
            top.x,
            top.y,
            bottom.z
    );

    glm::vec3 topFrontLeft = glm::vec3(
            bottom.x,
            top.y,
            top.z
    );

    glm::vec3 topFrontRight = glm::vec3(
            top.x,
            top.y,
            top.z
    );

    glm::vec3 normalLeft = glm::triangleNormal(bottomFrontLeft,topBackLeft,topFrontLeft);
    glm::vec3 normalRight = normalLeft * -1.0f;
    glm::vec3 normalBottom = glm::triangleNormal(bottomBackLeft, bottomBackRight, bottomFrontLeft);
    glm::vec3 normalTop = normalBottom * -1.0f;
    glm::vec3 normalFront = glm::triangleNormal(topFrontLeft,bottomFrontRight,bottomFrontLeft);
    glm::vec3 normalBack = normalFront * -1.0f;

    normals[0] = normalLeft;
    normals[1] = normalRight;
    normals[2] = normalBottom;
    normals[3] = normalTop;
    normals[4] = normalFront;
    normals[5] = normalBack;
}

bool IBounded::isWithinBounds(const BoundingRect &rect) const {
    BoundingRect boundedRect = getBoundingRect();

    if (boundedRect.bottom.x < rect.bottom.x ) {
        return false;
    }

    if (boundedRect.top.x > rect.top.x) {
        return false;
    }

    if (boundedRect.bottom.y < rect.bottom.y ) {
        return false;
    }

    if (boundedRect.top.y > rect.top.y) {
        return false;
    }

    if (boundedRect.bottom.z < rect.bottom.z ) {
        return false;
    }

    if (boundedRect.top.z > rect.top.z) {
        return false;
    }

    return true;
}
