#include "screen_root.h"
#include "glm/gtc/random.hpp"
#include "shader_manager.h"
#include "work_queue.h"
#include "shaders.h"
#include <camera.h>

ScreenRoot::ScreenRoot(ShaderManager * shaderManager, GLuint number):numberOfStars(number) {
    generatePoints(numberOfStars);

    vector<string> uniforms;
    uniforms.push_back(SHADER_UNIFORM_PROJECTION_MATRIX);

    vector<string> attribs;
    attribs.push_back(SHADER_ATTRIBUTE_POSITION);

    shader = shaderManager->createShader(getRelativeLocation("","/shaders/stars.vert"),getRelativeLocation("","/shaders/stars.frag"),attribs,uniforms);

    xDeg = 0.0f;
    yDeg = 0.0f;
}

void ScreenRoot::generatePoints(GLuint number) {
    GLfloat * pointVec = new GLfloat[number * 3];

    for (int i = 0; i < number; i++ ) {
        glm::vec3 currentNumber(glm::vec3(glm::ballRand(2.5f)));
        pointVec[i * 3 ] = currentNumber.x;
        pointVec[(i * 3) + 1] = currentNumber.y;
        pointVec[(i * 3) + 2] = currentNumber.z-3;
    }

    GLushort * shortBuffer = new GLushort[number];
    for ( size_t i = 0; i < number; i++ ) {
        shortBuffer[i]=(GLushort) i;
    }

    glGenBuffers(1, &vbo);
    ALOGI("Creating buffer: %d %d\n", number, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, number * 3 * sizeof(GLfloat), pointVec, GL_STATIC_DRAW);
    delete [] pointVec;

    glGenBuffers(1, &indexBuffer);
    ALOGI("index buffer: %d\n", indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * number, shortBuffer, GL_STATIC_DRAW);
    delete [] shortBuffer;
}

void ScreenRoot::drawNode(meow_time_t time, ShaderManager *shaderManager, const glm::mat4 &model,
              const glm::mat4 &view, const glm::mat4 &projectionView, const glm::vec3 &eyePos,
              const glm::vec3 &lookAt) {
    glUseProgram(shaderManager->getShaderProgram(shader));
    glDisable(GL_DEPTH_TEST);
    GLint attribPosition = shaderManager->getAttrib(shader,SHADER_ATTRIBUTE_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3,(GLvoid * ) 0);
    checkGlError("attribPosition->glVertexAttribPointer");
    glEnableVertexAttribArray(attribPosition);
    checkGlError("attribPosition_root->glEnableVertexAttribArray");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glDrawElements(GL_POINTS, numberOfStars, GL_UNSIGNED_SHORT, (void*)0);

    clearBoundBuffers();
    glEnable(GL_DEPTH_TEST);
}

void ScreenRoot::drawAfter(meow_time_t time, ShaderManager * shaderManager, const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projectionView, const glm::vec3 & eyePos) {
//    if (hasLine) {
//        glUseProgram(shaderManager->getShaderProgram(shader));
//        GLint attribPosition = shaderManager->getAttrib(shader,"a_Position");
//
//        glBindBuffer(GL_ARRAY_BUFFER,lineBuffer);
//        glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
//        glDrawArrays(GL_LINES,0,2);
//
//        clearBoundBuffers();
//    }
}

void ScreenRoot::rotateScene(GLfloat x, GLfloat y) {
    xDeg = x;
    yDeg = y;
}

void ScreenRoot::setLine(glm::vec3 vecStart, glm::vec3 vecEnd) {
    if ( hasLine ) {
        glDeleteBuffers(1, &lineBuffer);
    }

    hasLine = GL_TRUE;

    GLfloat position[6];

    position[0] = vecStart.x;
    position[1] = vecStart.y;
    position[2] = vecStart.z;
    position[3] = vecEnd.x;
    position[4] = vecEnd.y;
    position[5] = vecEnd.z;

    glGenBuffers(1, &lineBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, lineBuffer);
    glBufferData(GL_ARRAY_BUFFER, 2 * 3 * sizeof(GLfloat), position, GL_STATIC_DRAW);

}

glm::mat4 ScreenRoot::transformModelView(const glm::mat4 & model) const {
    return model * glm::orientate4(glm::vec3((float) xDeg / 5.0f,(float) yDeg / 5.0f,0.0f));
}

ScreenRoot::~ScreenRoot() {
    glDeleteBuffers(1,&vbo);
    glDeleteBuffers(1,&indexBuffer);
}

void ScreenRoot::setCamera(Camera *camera) {
    addNode(camera);
    this->camera = camera;
}

void ScreenRoot::processNode(meow_time_t time, engine * gameEngine) {
    UNUSED(gameEngine);
    UNUSED(time);
}
