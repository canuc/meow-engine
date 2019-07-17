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

#include "game.h"
#include "meow.h"
#include "screen_root.h"
#include "model_manager.h"
#include "main_character.h"
#include "font_manager.h"
#include "camera.h"
#include "scene_container.h"
#include "timer/timer.h"
#include "animation/animation_manager.h"
#include "gamethread.h"
#include "work_queue.h"
#include "texture_loading.h"
#include "events/events.h"
#include "unistd.h"
#include "platform_input.h"
#include "meow_util.h"
#include "variable_manager.h"
#include "md5/md5_model.h"

#define USLEEP_TIME 50000
const GLfloat Game::square[4 * 2] = {-1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f};
const GLfloat Game::squareTextureCoords[4 * 2] = {0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f};
const GLushort Game::indexes[6] = {0, 1, 2, 1, 2, 3};

#define RELATIVE_TEXTURE_DIMENSION(a,density) ((GLsizei) (a) / pow(2.0f,((density) / 160.0f)))
Game::Game(engine *gameEngine) : frames(0), framesPerSecond(0.0f) {
    this->gameEngine = gameEngine;
    attachments[0] = GL_COLOR_ATTACHMENT0;
    attachments[1] = GL_COLOR_ATTACHMENT1;
}

Game::~Game() {
    shutdown();
}

void Game::shutdown() {
    gameEngine->workQueue->addEvent(new ShutdownEvent());

    delete gameEngine->drawableState->character;
    delete gameEngine->drawableState->sceneGraph;
    delete gameEngine->drawableState->shaderManager;
    delete gameEngine->drawableState->modelManager;
    delete gameEngine->drawableState->textureManager;
    delete gameEngine->drawableState->timerManager;
    delete gameEngine->drawableState->screenContainer;
    delete gameEngine->drawableState->camera;
    delete gameEngine->drawableState->userController;
    delete gameEngine->drawableState->animationManager;
    delete gameEngine->drawableState->variableManager;
    delete gameEngine->drawQueue;

    gameEngine->drawableState->character = NULL;
    gameEngine->drawableState->sceneGraph = NULL;
    gameEngine->drawableState->shaderManager = NULL;
    gameEngine->drawableState->modelManager = NULL;
    gameEngine->drawableState->screenContainer = NULL;
    gameEngine->drawableState->textureManager = NULL;
    gameEngine->drawableState->camera = NULL;
    gameEngine->drawableState->timerManager = NULL;
    gameEngine->drawableState->fontManager = NULL;
    gameEngine->drawableState->userController = NULL;
    gameEngine->drawableState->animationManager = NULL;
    gameEngine->drawableState->variableManager = NULL;

    delete gameEngine->drawableState;
    delete[] gameEngine->scriptLocation;
    delete coconut;
    coconut = NULL;

    gameEngine->drawableState = NULL;
    gameEngine->drawQueue = NULL;
    gameEngine->scriptLocation = NULL;

    glDeleteFramebuffers(2,pingpongFBO);
    glDeleteFramebuffers(1,&framebuffer);
    glDeleteTextures(2,colorBuffers);
    glDeleteTextures(2,pingpongColorbuffers);
    glDeleteBuffers(3,screenVbo);
}

void Game::bindVars(VariableManager *var) const {
    var->addVariable("fps", new FunctionVariable([this]() {
        return to_string((int) round(framesPerSecond));
    }));
}

void Game::initializeHDR() {

    glGenBuffers(3, screenVbo);
    checkGlError("Game->glGenBuffers");
    glBindBuffer(GL_ARRAY_BUFFER, screenVbo[0]);
    checkGlError("Game[GL_ARRAY_BUFFER]->glBindBuffer");
    glBufferData(GL_ARRAY_BUFFER, sizeof(square), (GLvoid *) square, GL_STATIC_DRAW);
    checkGlError("Game[GL_ARRAY_BUFFER]->glBufferData");
    glBindBuffer(GL_ARRAY_BUFFER, screenVbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareTextureCoords), (GLvoid *) squareTextureCoords,
                 GL_STATIC_DRAW);
    checkGlError("Game[GL_ARRAY_BUFFER]->glBufferData");
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screenVbo[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), (GLvoid *) indexes, GL_STATIC_DRAW);
    checkGlError("Game->glBindBuffer");

    vector<string> stringAttributes = vector<string>();
    stringAttributes.push_back(SHADER_ATTRIBUTE_POSITION);
    stringAttributes.push_back(SHADER_ATTRIBUTE_TEX_COORD);

    vector<string> stringUniforms = vector<string>();
    stringUniforms.push_back(SHADER_UNIFORM_TEXTURE_ID);
    stringUniforms.push_back(SHADER_UNIFORM_BUMP_ID);

    shaderId = gameEngine->drawableState->shaderManager->createShader("/shaders/screen.vert",
                                                                      "/shaders/screen.frag",
                                                                      stringAttributes,
                                                                      stringUniforms);

    vector<string> blurStringAttributes = vector<string>();
    blurStringAttributes.push_back(SHADER_ATTRIBUTE_POSITION);
    blurStringAttributes.push_back(SHADER_ATTRIBUTE_TEX_COORD);

    vector<string> blurStringUniforms = vector<string>();
    blurStringUniforms.push_back(SHADER_UNIFORM_TEXTURE_ID);
    blurStringUniforms.push_back(SHADER_UNIFORM_BLUR_HORIZONTAL);

    blurShaderId = gameEngine->drawableState->shaderManager->createShader("/shaders/blur.vert",
                                                                      "/shaders/blur.frag",
                                                                      blurStringAttributes,
                                                                      blurStringUniforms);

    glGenFramebuffers(1, &framebuffer);

    checkGlError("Game->glGenFramebuffers");
    glGenTextures(2, colorBuffers);
    checkGlError("Game->glGenTextures");

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    for (GLuint i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RGB16F, gameEngine->width, gameEngine->height, 0, GL_RGB,
                GL_FLOAT, NULL
        );
        checkGlError("Game->glTexImage2D");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D(
                GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0
        );
        checkGlError("Game->glFramebufferTexture2D");
    }

    glGenRenderbuffers(1, &renderBuffer);
    checkGlError("Game->glGenRenderbuffers");
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    checkGlError("Game->glBindRenderbuffer");
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, gameEngine->width, gameEngine->height);
    ALOGI("Get width: %d height: %d, Density: %f",gameEngine->width, gameEngine->height, (10.0f * ((gameEngine->density) / 160.0f)));
    checkGlError("Game->glRenderbufferStorage");
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
    checkGlError("Game->glFramebufferRenderbuffer");

    glDrawBuffers(2, attachments);
    checkGlError("Game->glDrawBuffers");
    while (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        ALOGE("Framebuffer game not complete main!");
        usleep(USLEEP_TIME);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (GLuint i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, RELATIVE_TEXTURE_DIMENSION(gameEngine->width,gameEngine->density), RELATIVE_TEXTURE_DIMENSION(gameEngine->height,gameEngine->density), 0, GL_RGB, GL_FLOAT, NULL);

        if (i < 0) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // We clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);

        // Also check if framebuffers are complete (no need for depth buffer)
        while (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            checkGlError("Game->glCheckFramebufferStatus[pingpongFBO]");
            usleep(USLEEP_TIME);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void Game::bootstrap() {
    ShaderManager *shaderManager = new ShaderManager(gameEngine);
    ModelManager *modelManager = new ModelManager(shaderManager);
    Camera *camera = new Camera();
    ScreenRoot *root = new ScreenRoot(shaderManager, 800);
    checkGlError("engine_init_scene[ScreenRoot]");

    SceneContainer *container = new SceneContainer();
    TextureManager *textureManager = new TextureManager(gameEngine);
    MainCharacter *character = new MainCharacter("/characters/nyan.nync", modelManager,
                                                 shaderManager, textureManager);
    coconut = new Actor("/characters/coconut.nync", modelManager,
                                                 shaderManager, textureManager);
    checkGlError("engine_init_scene[MainCharacter]");
    TimerManager *timerManager = new TimerManager();
    FontManager *fontManager = new FontManager(shaderManager, gameEngine->width, gameEngine->height,
                                               gameEngine->density);
    IUserControl *inputManager = getPlatformControl(gameEngine);
    VariableManager *variable = new VariableManager();
    AnimationManager *animation = new AnimationManager();

    character->bindVars(variable);
    bindVars(variable);


    gameEngine->animating = 1;

    checkGlError("engine_init_scene[inputManager]");
    camera->addNode(container);
    camera->addNode(character);

    // Add coconut to the scene
    camera->addNode(coconut);

    root->addNode(camera);

    gameEngine->drawableState = new drawable_state();
    gameEngine->drawableState->character = character;
    gameEngine->drawableState->sceneGraph = root;
    gameEngine->drawableState->shaderManager = shaderManager;
    gameEngine->drawableState->modelManager = modelManager;
    gameEngine->drawableState->screenContainer = container;
    gameEngine->drawableState->textureManager = textureManager;
    gameEngine->drawableState->camera = camera;
    gameEngine->drawableState->timerManager = timerManager;
    gameEngine->drawableState->fontManager = fontManager;
    gameEngine->drawableState->userController = inputManager;
    gameEngine->drawableState->animationManager = animation;
    gameEngine->drawableState->variableManager = variable;

    gameEngine->scriptLocation = getScriptLocation(gameEngine);

    initializeFreetype();
    intializeQueues();
    initializeHDR();
    /////////////////////////////////////////////////////////
    //         MD5Model Test
    /////////////////////////////////////////////////////////
//    monkeyTexture = textureManager->loadTexture("/textures/mnkycolorbake.png");
//    mainCharacter = new MD5Model();
//    mainCharacter->loadMd5("/models/monkey.md5mesh");
//    mainCharacter->loadAnim("/models/monkey-forward.md5anim");
//
//    vector<string> attributes = {
//            SHADER_ATTRIBUTE_POSITION,
//            SHADER_ATTRIBUTE_COLOR,
//            SHADER_ATTRIBUTE_TEX_COORD,
//            SHADER_ATTRIBUTE_NORMAL,
//            SHADER_ATTRIBUTE_BITANGENT,
//            SHADER_ATTRIBUTE_TANGENT,
//            SHADER_ATTRIBUTE_MATRIX_INDEX,
//            SHADER_ATTRIBUTE_WEIGHT
//    };
//
//    vector<string> uniforms = {
//            SHADER_UNIFORM_TEXTURE_ID,
//            SHADER_UNIFORM_BONE_MATRIX,
//            SHADER_UNIFORM_PROJECTION_MATRIX,
//            SHADER_UNIFORM_MODEL_MATRIX,
//            SHADER_UNIFORM_VIEW_MATRIX,
//            SHADER_UNIFORM_TIME
//    };
//
//    md5ShaderId = shaderManager->createShader("/shaders/md5.vert", "/shaders/md5.frag", attributes, uniforms);
    ALOGI("Got shaderId: %d", md5ShaderId);

    /////////////////////////////////////////////////////////
    //          We can now use the queues
    ///////////////////////s//////////////////////////////////
    gameEngine->game_state = STATE_UNINITIALIZED;
    gameEngine->workQueue->addEvent(new StartEvent(character));


}

void Game::intializeQueues() {
    /////////////////////////////////////////////////////////
    //         Initialize the work queues
    /////////////////////////////////////////////////////////
    gameEngine->workQueue = new WorkQueue<LuaEvent>();
    gameEngine->drawQueue = new WorkQueue<DrawableEvent>();
    /////////////////////////////////////////////////////////
    //          We can now use the queues
    ///////////////////////s//////////////////////////////////
    start_game_thread(this->gameEngine);
}

void Game::initializeFreetype() {
    gameEngine->drawableState->fontManager->loadFont("/fonts/Champagne_Bold.ttf", 6);
    gameEngine->drawableState->fontManager->loadFont("/fonts/SigmarOne.ttf", 12);
    gameEngine->drawableState->fontManager->loadFont("/fonts/digital-7.ttf", 8);
    gameEngine->drawableState->fontManager->loadFont("/fonts/ARCADECLASSIC.TTF", 10);
}

void Game::drawframe() {
    if (gameEngine->game_state == STATE_GAME_SHOULD_RESTART) {
        shutdown();

        while (!getIsShutdown()) {
            usleep(USLEEP_TIME);
        }

        bootstrap();
    }

    //  Done with events; draw next animation frame. iff the unlock event has been sent
    if (gameEngine->animating && IS_UNLOCKED(gameEngine->game_state)) {
        glm::mat4 perpective = glm::perspective(45.0f, (GLfloat) ((GLfloat) gameEngine->width /
                                                                  (GLfloat) gameEngine->height),
                                                0.01f, 100.0f);
        glm::vec3 eyePos =
                gameEngine->drawableState->character->getPosition() + glm::vec3(0.0, 0.0, -3.0);
        glm::mat4 view = glm::lookAt(eyePos, gameEngine->drawableState->character->getPosition(),
                                     glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 model;

        frame(model, view, perpective, eyePos);
    } else {
        ALOGE("SLEEPING! GAME STATE! %d", gameEngine->game_state);
        usleep(USLEEP_TIME);
    }
}

void Game::frame(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &perspective,
                 const glm::vec3 &eyePos) {

    static meow_time_t lastTimeEvent = currentTimeInMilliseconds();
    meow_time_t currentTimeEvent = currentTimeInMilliseconds();

    frames++;
    if (lastTimeEvent + 1000L < currentTimeEvent) {
        float timeDiff = ((float) (currentTimeEvent - lastTimeEvent)) / 1000.0f;
        framesPerSecond = ((float) frames) / timeDiff;
        frames = 0;
        lastTimeEvent = currentTimeEvent;
    }

    gameEngine->drawableState->userController->pollInput(currentTimeEvent);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gameEngine->drawableState->sceneGraph->rotateScene(gameEngine->state.rotation.x,
                                                       gameEngine->state.rotation.y);

    gameEngine->drawableState->sceneGraph->draw(currentTimeInMilliseconds(),
                                                gameEngine->drawableState->shaderManager, model,
                                                view,
                                                perspective, eyePos, glm::vec3());

    gameEngine->drawableState->animationManager->animate(currentTimeEvent);
    /**
     * TEST CODE:
     *
     * This is to bootstrap the MD5Model.
     * {
     */
    checkGlError("MD5::before::glUseProgram");
    glUseProgram(gameEngine->drawableState->shaderManager->getShaderProgram(md5ShaderId));

    // glUseProgram
    checkGlError("MD5::glUseProgram");
    glActiveTexture(GL_TEXTURE0);
    checkGlError("glActiveTexture");
    glBindTexture(GL_TEXTURE_2D, monkeyTexture);
    checkGlError("glBindTexture");
    glUniform1i(gameEngine->drawableState->shaderManager->getUniform(md5ShaderId,SHADER_UNIFORM_TEXTURE_ID),0);
    checkGlError("glBindTexture");
    // End: glUseProgram


    /**
     * }
     */

    flush(gameEngine);

    // After the draw has occured then continue the timer manager
    gameEngine->drawableState->timerManager->run(currentTimeEvent, gameEngine);
    gameEngine->drawableState->sceneGraph->processNodeEvents(currentTimeEvent, gameEngine);
}

void Game::restart() {
    gameEngine->game_state = STATE_GAME_SHOULD_RESTART;
}

void Game::flush(engine *gameEngine) {
    flushFBO(gameEngine);
}

void Game::flushFBO(engine *gameEngine) {
    GLboolean horizontal = true, first_iteration = true;
    ShaderManager *shaderManager = gameEngine->drawableState->shaderManager;

    for (int i = 0; i < 4; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        glViewport( 0, 0, RELATIVE_TEXTURE_DIMENSION(gameEngine->width, gameEngine->density), RELATIVE_TEXTURE_DIMENSION(gameEngine->height, gameEngine->density) );
        glClear(GL_COLOR_BUFFER_BIT);

        checkGlError("flushFBO->glBindFramebuffer[pingpongFBO]");
        glUseProgram(shaderManager->getShaderProgram(blurShaderId));
        checkGlError("flushFBO->glUseProgram[pingpongFBO]");

        GLint textureUniform = shaderManager->getUniform(blurShaderId, SHADER_UNIFORM_TEXTURE_ID);

        if (shaderManager->hasUniform(blurShaderId, SHADER_UNIFORM_TEXTURE_ID)) {
            glActiveTexture(GL_TEXTURE0);
            checkGlError("glActiveTexture[pingpongFBO]");
            glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);
            checkGlError("glBindTexture[pingpongFBO]");
            glUniform1i(textureUniform, 0);
            checkGlError("flushFBO->glUniform1i[pingpongFBO]");
        }

        if (shaderManager->hasUniform(blurShaderId, SHADER_UNIFORM_BLUR_HORIZONTAL)) {
            glUniform1i(shaderManager->getUniform(blurShaderId, SHADER_UNIFORM_BLUR_HORIZONTAL), horizontal);
            checkGlError("flushFBO->glUniform1i[pingpongFBO,horizontal]");
        }

        drawFullscreen(gameEngine, blurShaderId);

        horizontal = !horizontal;

        if (first_iteration) {
            first_iteration = false;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport( 0, 0, (GLsizei) gameEngine->width, (GLsizei) gameEngine->height );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderManager->getShaderProgram(shaderId));

    GLint textureUniform = shaderManager->getUniform(shaderId, SHADER_UNIFORM_TEXTURE_ID);

    if (shaderManager->hasUniform(shaderId, SHADER_UNIFORM_TEXTURE_ID)) {
        glActiveTexture(GL_TEXTURE0);
        checkGlError("glActiveTexture");
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        checkGlError("glBindTexture");
        glUniform1i(textureUniform, 0);
        checkGlError("flushFBO->glUniform1i[u_texture]");
    }

    GLint bumpUniform = shaderManager->getUniform(shaderId, SHADER_UNIFORM_BUMP_ID);

    if (shaderManager->hasUniform(shaderId, SHADER_UNIFORM_BUMP_ID)) {
        glActiveTexture(GL_TEXTURE1);
        checkGlError("glActiveTexture[1]");
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
        checkGlError("glBindTexture");
        glUniform1i(bumpUniform, 1);
        checkGlError("flushFBO->glUniform1i[u_bump]");
    }

    drawFullscreen(gameEngine, shaderId);
}

void Game::drawFullscreen(engine *gameEngine, int fullShaderId) {
    ShaderManager *shaderManager = gameEngine->drawableState->shaderManager;

    GLint positionAttribute = shaderManager->getAttrib(fullShaderId, SHADER_ATTRIBUTE_POSITION);
    GLint textureAttribute = shaderManager->getAttrib(fullShaderId, SHADER_ATTRIBUTE_TEX_COORD);

    glBindBuffer(GL_ARRAY_BUFFER, screenVbo[0]);
    glVertexAttribPointer((GLuint) positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0);
    glEnableVertexAttribArray((GLuint) positionAttribute);
    checkGlError("flushFBO->glVertexAttribPointer[a_Position]");

    if (shaderManager->hasAttrib(fullShaderId, SHADER_ATTRIBUTE_TEX_COORD)) {
        glBindBuffer(GL_ARRAY_BUFFER, screenVbo[1]);
        glVertexAttribPointer((GLuint) textureAttribute, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0);
        glEnableVertexAttribArray((GLuint) textureAttribute);
        checkGlError("flushFBO->glVertexAttribPointer[v_texCoord]");
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screenVbo[2]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void *) 0);
    checkGlError("flushFBO->glDrawElements");
}
