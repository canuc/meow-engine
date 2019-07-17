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

#ifndef NYAN_ANDROID_CAMERA_H_H
#define NYAN_ANDROID_CAMERA_H_H

#include <glm/vec3.hpp>
#include "inode.h"
#include <stack>
#include "drawable.h"

typedef enum {
    CAMERA_MODE_ORIGIN,
    CAMERA_FOLLOWING_MODE,
    CAMERA_ANIMATION_MODE
} t_camera_mode;

typedef enum {
    CAMERA_STATE_CONTINUE,
    CAMERA_STATE_POP
} t_camera_return;

class Actor;

class CameraState {
public:
    CameraState(t_camera_mode cameraMode): mode(cameraMode) {}
    virtual ~CameraState() {}
    t_camera_mode getMode() { return mode; }

    virtual t_camera_return calculateCamera(meow_time_t currentTime, glm::mat4 * vec, glm::vec3 * eye, glm::vec3 * lookAt, const glm::vec3 & jitter)=0;
private:
    const t_camera_mode mode;
};

class OriginState: public CameraState {
public:
    OriginState(glm::vec3 cameraOffset);

    virtual t_camera_return calculateCamera(meow_time_t currentTime, glm::mat4 * viewMatrix, glm::vec3 * eye, glm::vec3 * lookAt, const glm::vec3 & jitter);
private:
    glm::vec3 camera;
};

class FollowingState: public CameraState {
public:
    FollowingState(Actor * character, glm::vec3 cameraOffset);

    virtual t_camera_return calculateCamera(meow_time_t currentTime, glm::mat4 * viewMatrix, glm::vec3 * eye, glm::vec3 * lookAt, const glm::vec3 & jitter);
private:
    Actor * followingCharacter;
    glm::vec3 offset;
};

class AnimationState: public CameraState {
public:
    AnimationState(glm::vec3 start, glm::vec3 end, glm::vec3 offset, meow_time_t animDuration);
    AnimationState(glm::vec3 start, glm::vec3 end, glm::vec3 offsetStart, glm::vec3 offsetEnd, meow_time_t animDuration);

    virtual t_camera_return calculateCamera(meow_time_t currentTime, glm::mat4 * viewMatrix, glm::vec3 * eye, glm::vec3 * lookAt, const glm::vec3 & jitter);

private:
    glm::vec3 animationStart;
    glm::vec3 animationEnd;

    glm::vec3 cameraOffsetStart;
    glm::vec3 cameraOffsetEnd;

    meow_time_t duration;
    meow_time_t startTime;
    bool animateOffset;
};

class Camera:public INode {
public:
    Camera();
    virtual ~Camera();

    void addState(CameraState * state);
    void draw(meow_time_t time, ShaderManager * shaderManager,const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projectionView, const glm::vec3 & eyePos, const glm::vec3 & lookAt);

    void setJitter(glm::vec3 jitter);
    glm::vec3 jitter() const;
protected:
    void drawNode(meow_time_t time, ShaderManager *shaderManager, const glm::mat4 &model,
                  const glm::mat4 &view, const glm::mat4 &projectionView, const glm::vec3 &eyePos,
                  const glm::vec3 &lookAt);
    void updatePosition();


private:
    glm::vec3 cameraPosition;
    glm::vec3 cameraOffset;
    glm::vec3 cameraJitter;
    Actor * followingCharacter;
    t_camera_mode mode;

    std::stack<CameraState *> stateStack;
};


#endif //NYAN_ANDROID_CAMERA_H_H
