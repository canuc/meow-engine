#include "parallel_plane.h"

ParallelPlane::ParallelPlane(t_plane_direction direction, float start):planeDirection(direction), planeStart(start) {

}

void ParallelPlane::drawNode(meow_time_t time, ShaderManager *shaderManager, const glm::mat4 &model,
                         const glm::mat4 &view, const glm::mat4 &projectionView,
                         const glm::vec3 &eyePos, const glm::vec3 &lookAt) {
    glm::vec4 lookAtViewSpace = view * glm::vec4(lookAt,1.0f);
    glm::vec4 leftPoint(-1, 0, lookAtViewSpace.z / lookAtViewSpace.w, 1);
    glm::vec4 rightPoint(1, 0, lookAtViewSpace.z / lookAtViewSpace.w, 1);
    glm::vec4 topPoint(0, 1, lookAtViewSpace.z / lookAtViewSpace.w, 1);
    glm::vec4 bottomPoint(0, -1, lookAtViewSpace.z / lookAtViewSpace.w, 1);
    glm::mat4 inverseProjection = glm::inverse(projectionView * view);

    topPoint = inverseProjection * topPoint;
    bottomPoint = inverseProjection * bottomPoint;
    leftPoint = inverseProjection * leftPoint;
    rightPoint = inverseProjection * leftPoint;

    glm::vec3 bottomCamera = glm::vec3(bottomPoint) /  bottomPoint.w;
    glm::vec3 topCamera = glm::vec3(topPoint) /  topPoint.w;
    glm::vec3 leftCamera = glm::vec3(leftPoint) /  leftPoint.w;
    glm::vec3 rightCamera = glm::vec3(rightPoint) /  rightPoint.w;

    //ALOGE("RIGHT: %f LEFT: %f TOP: %f BOTTOM: %+f",right,left,top,bottom);

    // found this method from: http://gamedev.stackexchange.com/questions/29999/how-do-i-create-a-bounding-frustum-from-a-view-projection-matrix
    if (planeDirection == NEGATIVE_Y && bottomCamera.y < planeStart) {
        glm::vec3 normal = glm::normalize(lookAt - eyePos);
        //float equationOfPlane = topRight.x * normal.x + topRight.y * normal.y + topRight.z * normal.z;

        ALOGE("SHOULD SHOW BOTTOM!! %f",bottomCamera.y  );
    }
}
