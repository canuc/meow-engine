#include "camera.h"
#include "actor.h"

Camera::Camera():cameraOffset(glm::vec3(0.0f,0.0f,-3.0f)),followingCharacter(NULL),mode(CAMERA_MODE_ORIGIN) {
}

Camera::~Camera() {
}

void Camera::updatePosition() {
}

void Camera::draw(meow_time_t time, ShaderManager * shaderManager,const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projectionView, const glm::vec3 & eyePos, const glm::vec3 & lookAt) {

    glm::mat4 viewMatrix = view;
    glm::vec3 eye = eyePos;
    glm::vec3 lookAtPosition = lookAt;

    if ( stateStack.size()) {
        CameraState * state = stateStack.top();
        t_camera_return result = state->calculateCamera(time, &viewMatrix, &eye, &lookAtPosition, cameraJitter);

        if ( result == CAMERA_STATE_POP) {
            stateStack.pop();

            delete state;
        }
    }

    INode::draw(time, shaderManager, model, viewMatrix, projectionView, eye + cameraJitter, lookAtPosition);
}

//void Camera::getEyePosition(meow_time_t time, )

void Camera::drawNode(meow_time_t time, ShaderManager *shaderManager, const glm::mat4 &model,
              const glm::mat4 &view, const glm::mat4 &projectionView, const glm::vec3 &eyePos,
              const glm::vec3 &lookAt) {
    UNUSED(time);
    UNUSED(shaderManager);
    UNUSED(model);
    UNUSED(view);
    UNUSED(projectionView);
    UNUSED(eyePos);
    UNUSED(lookAt);
}

void Camera::addState(CameraState *state) {
    stateStack.push(state);
}

void Camera::setJitter(glm::vec3 jitter) {
    cameraJitter = jitter;
}

glm::vec3 Camera::jitter() const {
    return cameraJitter;
}

OriginState::OriginState(glm::vec3 cameraOffset):CameraState(CAMERA_MODE_ORIGIN), camera(cameraOffset) {

}

t_camera_return OriginState::calculateCamera(meow_time_t currentTime, glm::mat4 *viewMatrix, glm::vec3 * eye, glm::vec3 * lookAt, const glm::vec3 & jitter) {
    UNUSED(currentTime);
    *lookAt = glm::vec3(0.0f,0.0f,0.0f);
    *eye = camera + jitter;
    *viewMatrix = glm::lookAt(*eye,*lookAt,glm::vec3(0.0f,1.0f,0.0f));


    return CAMERA_STATE_CONTINUE;
}

FollowingState::FollowingState(Actor *character, glm::vec3 cameraOffset):CameraState(CAMERA_FOLLOWING_MODE),followingCharacter(character),offset(cameraOffset) {

}

t_camera_return FollowingState::calculateCamera(meow_time_t currentTime, glm::mat4 *viewMatrix, glm::vec3 * eye, glm::vec3 * lookAt, const glm::vec3 & jitter) {
    *eye = followingCharacter->getPosition() - offset + jitter;
    *lookAt = followingCharacter->getPosition();
    *viewMatrix = glm::lookAt(*eye,*lookAt,glm::vec3(0.0f,1.0f,0.0f));

    return CAMERA_STATE_CONTINUE;
}

AnimationState::AnimationState(glm::vec3 start, glm::vec3 end,
                               glm::vec3 offset, meow_time_t animDuration):CameraState(CAMERA_ANIMATION_MODE) {
    animateOffset = false;
    animationStart = start;
    animationEnd = end;
    cameraOffsetStart = offset;
    duration = animDuration;
    startTime = 0;
}

AnimationState::AnimationState(glm::vec3 start, glm::vec3 end,
                               glm::vec3 offsetStart, glm::vec3 offsetEnd,
                               meow_time_t animDuration):CameraState(CAMERA_ANIMATION_MODE) {
    animateOffset = true;
    animationStart = start;
    animationEnd = end;
    cameraOffsetStart = offsetStart;
    cameraOffsetEnd = offsetEnd;
    duration = animDuration;
    startTime = 0;
}

t_camera_return AnimationState::calculateCamera(meow_time_t currentTime, glm::mat4 *viewMatrix, glm::vec3 * eye, glm::vec3 * lookAt, const glm::vec3 & jitter) {
    if (startTime == 0) {
        startTime = currentTime;
    }

    meow_time_t time = currentTime - startTime;
    float percent = min((float) (time) / (float) duration, 1.0f);

    glm::vec3 position = animationStart + ((animationEnd - animationStart) * percent);
    glm::vec3 cameraOffset = cameraOffsetStart;

    if (animateOffset) {
        cameraOffset = cameraOffsetStart + ((cameraOffsetEnd - cameraOffsetStart) * percent);
    }

    glm::vec3 eyePos = position + cameraOffset + jitter;
    *eye = eyePos;
    *lookAt = position;
    *viewMatrix = glm::lookAt(eyePos,*lookAt,glm::vec3(0.0f,1.0f,0.0f));

    return startTime + duration > currentTime ? CAMERA_STATE_CONTINUE : CAMERA_STATE_POP;;
}

