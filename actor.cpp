
#include "actor.h"
#include "meow_util.h"
#include <sstream>
#include "model_manager.h"
#include <meow.h>

#include "platform_fopen.h"
#include "lualibs/characterlib.h"
#include "md5/md5_model.h"
#include "md5/md5_animation.h"


Actor::Actor(const string & file, ModelManager * models, ShaderManager * shaders, TextureManager *textures, PActorDescriptor * overrideDescriptor):
        accelerator(new Accelerator(&position)), textureManager(textures), shaderManager(shaders), modelManager(models)
{
    texID = 0;
    model = NULL;
    filename = file;
    vbo = NULL;
    loadCharacter(file, overrideDescriptor, shaderManager);
}

Actor::~Actor() {
    delete accelerator;
    if ( vbo != NULL) {
        glDeleteBuffers(2, vbo);
    }

    if ( parent ) {
        ALOGI("Deleting actor with parent: %p, %s",parent,filename.c_str());
    }
}


PActorDescriptor Actor::loadCharacterDescriptor(const string & file) {
    ALOGI("Loading character at: %s\n", file.c_str());

    FILE * rwOps = meow_fopen(file.c_str(),"r");

    string commandBuffer;
    string currentFilePath = getFilepath(file);

    PActorDescriptor characterDescriptor = createDescriptor();
    PActorDescriptor subDescriptor = NULL;

    if ( rwOps != NULL ) {
        char  buffer[LINE_BUFFER_SIZE] = {0};
        int result = readline(rwOps,buffer);

        while( result != EOF_SENTINEL ) {
            stringstream stringInputStream(buffer);
            stringInputStream >> commandBuffer;

            if (commandBuffer == TOKEN_CHARACTER_NAME) {
                stringInputStream >> characterDescriptor->name;
            } else if (commandBuffer == TOKEN_CHARACTER_SHADER_FRAGMENT) {
                stringInputStream >> characterDescriptor->fragment;
                characterDescriptor->fragment = getRelativeLocation(currentFilePath,
                                                                    characterDescriptor->fragment);
            } else if (commandBuffer == TOKEN_CHARACTER_SHADER_VERTEX) {
                stringInputStream >> characterDescriptor->vertex;
                characterDescriptor->vertex = getRelativeLocation(currentFilePath,
                                                                  characterDescriptor->vertex);
            } else if (commandBuffer == TOKEN_CHARACTER_SHADER_TEXTURE) {
                stringInputStream >> characterDescriptor->texture;
                characterDescriptor->texture = getRelativeLocation(currentFilePath,
                                                                   characterDescriptor->texture);
            } else if (commandBuffer == TOKEN_CHARACTER_MODEL) {
                stringInputStream >> characterDescriptor->model;
                characterDescriptor->model = getRelativeLocation(currentFilePath,
                                                                 characterDescriptor->model);
            } else if (commandBuffer == TOKEN_CHARACTER_SCALE) {
                stringInputStream >> characterDescriptor->scale;
            } else if (commandBuffer == TOKEN_CHARACTER_ROTATE) {
                stringInputStream >> characterDescriptor->rotation.x >>
                characterDescriptor->rotation.y >> characterDescriptor->rotation.z;
            } else if (commandBuffer == TOKEN_CHARACTER_POSITION) {
                stringInputStream >> characterDescriptor->position.x >>
                characterDescriptor->position.y >> characterDescriptor->position.z;
            } else if (commandBuffer == TOKEN_CHARACTER_MASS) {
                stringInputStream >> characterDescriptor->mass;
            } else if (commandBuffer == TOKEN_CHARACTER_BLENDING) {
                stringInputStream >> characterDescriptor->blending;
                characterDescriptor->hasBlending = true;
            } else if (commandBuffer == TOKEN_CHARACTER_BOUNDING) {
                stringInputStream >> characterDescriptor->circular;
                characterDescriptor->hasCircular = true;
            } else if (commandBuffer == TOKEN_CHARACTER_COLLISION) {
                stringInputStream >> characterDescriptor->collisionEnabled;
                characterDescriptor->hasCollision = true;
            } else if (commandBuffer == TOKEN_CHARACTER_BUMP_TEXTURE) {
                stringInputStream >> characterDescriptor->bump;
                characterDescriptor->bump = getRelativeLocation(currentFilePath,
                                                                characterDescriptor->bump);
            } else if (commandBuffer == TOKEN_CHARACTER_MD5_MODEL) {
                stringInputStream >> characterDescriptor->md5model;
            } else if (commandBuffer == TOKEN_CHARACTER_MD5_ANIM) {
                stringInputStream >> characterDescriptor->md5anim;
            } else if (commandBuffer == TOKEN_CHARACTER_INCLUDE) {
                string subInclude;
                stringInputStream >> subInclude;

                if (subDescriptor != NULL) {
                    delete subDescriptor;
                }

                subDescriptor = loadCharacterDescriptor(getRelativeLocation(currentFilePath, subInclude));
            } else if (commandBuffer == TOKEN_CHARACTER_UNIFORM) {
                string uniformName;
                float uniformValue;

                stringInputStream >> uniformName >> uniformValue;

                characterDescriptor->uniforms[uniformName] = uniformValue;
            }

            memset(buffer,0x00, LINE_BUFFER_SIZE);
            result = readline(rwOps,buffer);
        }

        if (subDescriptor) {
            mergeCharacterDescriptor(characterDescriptor, subDescriptor);

            delete subDescriptor;
        }

        fclose(rwOps);
    }

    return characterDescriptor;
}

PActorDescriptor Actor::createDescriptor() {
    PActorDescriptor characterDescriptor = new ActorDesciptor();

    characterDescriptor->scale = 1.0f;
    characterDescriptor->hasBlending = false;
    characterDescriptor->hasCircular = false;
    characterDescriptor->blending = false;
    characterDescriptor->circular = false;
    characterDescriptor->hasCollision = false;
    characterDescriptor->collisionEnabled = false;

    return characterDescriptor;
}

PActorDescriptor Actor::mergeCharacterDescriptor(PActorDescriptor characterDescriptor,
                                                 PActorDescriptor subDescriptor) {

    if (!characterDescriptor->name.size()) {
        characterDescriptor->name = subDescriptor->name;
    }

    if (!characterDescriptor->vertex.size()) {
        characterDescriptor->vertex = subDescriptor->vertex;
    }

    if (!characterDescriptor->fragment.size()) {
        characterDescriptor->fragment = subDescriptor->fragment;
    }

    if (!characterDescriptor->model.size()) {
        characterDescriptor->model = subDescriptor->model;
    }

    if (!characterDescriptor->texture.size()) {
        characterDescriptor->texture = subDescriptor->texture;
    }

    if (characterDescriptor->scale == 1.0f) {
        characterDescriptor->scale = subDescriptor->scale;
    }

    if (characterDescriptor->rotation.x == 0.0f && characterDescriptor->rotation.y == 0.0f && characterDescriptor->rotation.z == 0.0f) {
        characterDescriptor->rotation = subDescriptor->rotation;
    }

    if (characterDescriptor->position.x == 0.0f && characterDescriptor->position.y == 0.0f && characterDescriptor->position.z == 0.0f) {
        characterDescriptor->position = subDescriptor->position;
    }

    if (characterDescriptor->mass == 0.0f ) {
        characterDescriptor->mass = subDescriptor->mass;
    }

    if (!characterDescriptor->hasBlending) {
        characterDescriptor->blending = subDescriptor->blending;
    }

    if (!characterDescriptor->hasCircular) {
        characterDescriptor->circular = subDescriptor->circular;
    }

    if (!characterDescriptor->hasCollision) {
        characterDescriptor->collisionEnabled = subDescriptor->collisionEnabled;
    }

    if (!characterDescriptor->bump.size()) {
        characterDescriptor->bump = subDescriptor->bump;
    }

    if (!characterDescriptor->md5anim.size()) {
        characterDescriptor->md5anim = subDescriptor->md5anim;
    }

    if (!characterDescriptor->md5model.size()) {
        characterDescriptor->md5model = subDescriptor->md5model;
    }

    std::map<std::string,float>::iterator uniformIter = subDescriptor->uniforms.begin();
    while (uniformIter != subDescriptor->uniforms.end()) {
        if (characterDescriptor->uniforms.find(uniformIter->first) == characterDescriptor->uniforms.end()) {
            characterDescriptor->uniforms[uniformIter->first] = uniformIter->second;
        }
        uniformIter++;
    }

    return characterDescriptor;
}


void Actor::loadCharacter(const string & file, PActorDescriptor * descriptor, ShaderManager * shaderManager) {
    vector<string> stringAttributes = vector<string>();
    stringAttributes.push_back(SHADER_ATTRIBUTE_POSITION);
    stringAttributes.push_back(SHADER_ATTRIBUTE_COLOR);
    stringAttributes.push_back(SHADER_ATTRIBUTE_NORMAL);

    vector<string> stringUniforms = vector<string>();
    stringUniforms.push_back(SHADER_UNIFORM_MODEL_MATRIX);
    stringUniforms.push_back(SHADER_UNIFORM_PROJECTION_MATRIX);
    stringUniforms.push_back(SHADER_UNIFORM_VIEW_MATRIX);
    stringUniforms.push_back(SHADER_UNIFORM_EYE_VEC);
    stringUniforms.push_back(SHADER_UNIFORM_POSITION);
    stringUniforms.push_back(SHADER_UNIFORM_TIME);

    if (descriptor != NULL) {
        uniforms = (*descriptor)->uniforms;
        std::map<std::string, float>::iterator uniformIter = uniforms.begin();

        while (uniformIter != uniforms.end()) {
            char *uniformString = new char[uniformIter->first.length() + 1];
            memset(uniformString, 0x00, uniformIter->first.length() + 1);
            memcpy(uniformString, uniformIter->first.c_str(), uniformIter->first.length());
            stringUniforms.push_back(uniformIter->first.c_str());
            uniformIter++;
        }
    }

    PActorDescriptor actorDescriptor = loadCharacterDescriptor(file);

    if (descriptor) {
        PActorDescriptor releasebleDescriptor = actorDescriptor;
        actorDescriptor = mergeCharacterDescriptor(*descriptor, actorDescriptor);
        delete releasebleDescriptor;
        *descriptor = NULL;
    }

    hasTexture = false;
    hasBump = false;
    hasBlending = actorDescriptor->blending;

    if (actorDescriptor->md5model.size()) {
        MD5Model * loadingModel = new MD5Model();
        loadingModel->loadMd5(actorDescriptor->md5model);

        if (actorDescriptor->md5anim.size()) {
            loadingModel->loadAnim(actorDescriptor->md5anim);
        }
        model = loadingModel;
    } if (actorDescriptor->model.size()) {
        model = modelManager->loadModel(actorDescriptor->model);
    }

    if (actorDescriptor->texture.size()) {
        hasTexture = true;
        texID = loadTexture(actorDescriptor->texture);
        stringAttributes.push_back(SHADER_ATTRIBUTE_TEX_COORD);
        stringUniforms.push_back(SHADER_UNIFORM_TEXTURE_ID);
    }

    if (actorDescriptor->bump.size()) {
        hasBump = true;
        bumpID = loadTexture(actorDescriptor->bump);
        stringUniforms.push_back(SHADER_UNIFORM_BUMP_ID);

        // We only want to add to the uniform
        if (!hasTexture) {
            stringAttributes.push_back(SHADER_ATTRIBUTE_TEX_COORD);
        }
    }

    if (actorDescriptor->md5anim.size() && actorDescriptor->md5model.size()) {
        stringUniforms.push_back(SHADER_UNIFORM_BONE_MATRIX);
        stringAttributes.push_back(SHADER_ATTRIBUTE_WEIGHT);
        stringAttributes.push_back(SHADER_ATTRIBUTE_MATRIX_INDEX);
    }

    if (actorDescriptor->vertex.size() && actorDescriptor->fragment.size()) {
        ALOGI("Creating Shader: v: %s f: %s\n", actorDescriptor->vertex.c_str(), actorDescriptor->fragment.c_str());
        shaderID = shaderManager->createShader(actorDescriptor->vertex, actorDescriptor->fragment,stringAttributes,stringUniforms);
        if ( shaderID < 0 ) {
            ALOGI("ERROR CREATING SHADER: %d\n\n", shaderID);
        }
    }

    scale = actorDescriptor->scale;
    rotation = actorDescriptor->rotation;
    position = actorDescriptor->position;
    mass = actorDescriptor->mass;
    name = actorDescriptor->name;
    isCollisionEnabled = actorDescriptor->collisionEnabled;

    accelerator->setMass(actorDescriptor->mass);
    collision_model = actorDescriptor->circular ? COLLISION_MODEL_SPHERE : COLLISION_MODEL_BOUNDS;
    delete actorDescriptor;

    #ifdef DRAW_BOUNDS
    vector<string> shaderUniforms;
    shaderUniforms.push_back(SHADER_UNIFORM_PROJECTION_MATRIX);
    shaderUniforms.push_back(SHADER_UNIFORM_MODEL_MATRIX);
    shaderUniforms.push_back(SHADER_UNIFORM_VIEW_MATRIX);


    vector<string> attribs;
    attribs.push_back(SHADER_ATTRIBUTE_POSITION);

    boundingShader = shaderManager->createShader(getRelativeLocation("","/shaders/bounding_box.vert"),getRelativeLocation("","/shaders/bounding_box.frag"),attribs,shaderUniforms);
    #endif
}

GLuint Actor::loadTexture(const string & filename) {
    return textureManager->loadTexture(filename);
}

void Actor::processNode(meow_time_t time, engine * gameEngine) {
    // Only advance the character if the game has started and unlocked
    if (IS_UNLOCKED(gameEngine->game_state) && !IS_GAME_END(gameEngine->game_state)) {
        accelerator->calcOffset(time);
    }
}

const string &Actor::getName() const {
    return name;
}

bool Actor::collisionEnabled() const {
    return isCollisionEnabled;
}

void Actor::drawNode(meow_time_t time, ShaderManager * shaderManager, const glm::mat4 & modelMat, const glm::mat4 & view, const glm::mat4 & projectionView, const glm::vec3 & eyePos, const glm::vec3 &lookAt) {
    if (model) {
        if (hasBlending) {
            glEnable(GL_BLEND);
            glDisable(GL_DEPTH_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;
        }

        // bind the shader
        checkGlError("before::glUseProgram");
        glUseProgram(shaderManager->getShaderProgram(shaderID));
        checkGlError("glUseProgram");

        model->setShaderId(shaderID);

        if (hasTexture) {
            model->enableTexture(texID);
        } else {
            model->disableTexture();
        }

        if (hasBump) {
            model->enableBumpMap(bumpID);
        } else {
            model->disableBump();
        }

        if (shaderManager->hasUniform(shaderID,SHADER_UNIFORM_POSITION)) {
            // Bind the modelview & projection
            glUniform3fv(shaderManager->getUniform(shaderID, SHADER_UNIFORM_POSITION),1,glm::value_ptr(getPosition()));
            checkGlError(SHADER_UNIFORM_POSITION);
        }

        std::map<std::string,float>::iterator uniformIter = uniforms.begin();
        while (uniformIter != uniforms.end()) {
            if (shaderManager->hasUniform(shaderID,uniformIter->first.c_str())) {
                glUniform1f(shaderManager->getUniform(shaderID, uniformIter->first.c_str()), uniformIter->second);
            }
            uniformIter++;
        }

        #ifndef NO_DRAW_CHARACTERS // just see bounding box
            model->draw(time, shaderManager, modelMat, view, projectionView, eyePos, lookAt);
        #endif
        #ifdef DRAW_BOUNDS
            BoundingRect rect = getBoundingRect();
            BoundingSphere sphere = getBoundingSphere();
            loadBoundingBox(rect.bottom,rect.top,sphere.radius,rect.normals);
            drawBoundingBox(time,shaderManager,glm::mat4(),view,projectionView, getPosition() - eyePos);
        #endif
        if (hasBlending) {
            glDisable(GL_BLEND);
            glDisable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
        }
    }
}

void Actor::drawBoundingBox(meow_time_t time, ShaderManager * shaderManager, const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projectionView, const glm::vec3 & eyePos) {
    GLfloat currentLineWidth = 0.0f;
    glGetFloatv(GL_LINE_WIDTH,&currentLineWidth);
    glUseProgram(shaderManager->getShaderProgram(boundingShader));
    glLineWidth(5.0);
    GLint attribPosition = shaderManager->getAttrib(boundingShader,SHADER_ATTRIBUTE_POSITION);

    if (shaderManager->hasUniform(boundingShader,SHADER_UNIFORM_MODEL_MATRIX)) { // Bind the modelview matrix
        glUniformMatrix4fv(shaderManager->getUniform(boundingShader, SHADER_UNIFORM_MODEL_MATRIX), 1, GL_FALSE,
                           glm::value_ptr(model));
        checkGlError(SHADER_UNIFORM_MODEL_MATRIX);
    } else {
        ALOGE("ERROR no model matrix uniform! %s", filename.c_str());
    }

    if (shaderManager->hasUniform(boundingShader,SHADER_UNIFORM_VIEW_MATRIX)) { // Bind the view matrix
        glUniformMatrix4fv(shaderManager->getUniform(boundingShader, SHADER_UNIFORM_VIEW_MATRIX), 1, GL_FALSE,
                           glm::value_ptr(view));
        checkGlError(SHADER_UNIFORM_VIEW_MATRIX);
    } else {
        ALOGE("ERROR no view matrix uniform! %s", filename.c_str());
    }

    if ( shaderManager->hasUniform(boundingShader,SHADER_UNIFORM_PROJECTION_MATRIX)) { // Bind the projection matrix
        glUniformMatrix4fv(shaderManager->getUniform(boundingShader, SHADER_UNIFORM_PROJECTION_MATRIX), 1, GL_FALSE,
                           glm::value_ptr(projectionView));
        checkGlError(SHADER_UNIFORM_PROJECTION_MATRIX);
    } else {
        ALOGE("ERROR no perspective matrix uniform! %s", filename.c_str());
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer((GLuint) attribPosition, 3, GL_FLOAT, GL_FALSE, 0,(GLvoid * ) 0);
    checkGlError("drawBoundingBox->glVertexAttribPointer");
    glEnableVertexAttribArray((GLuint) attribPosition);
    checkGlError("drawBoundingBox->glEnableVertexAttribArray");
    int numberOfLines = 18;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    glDrawElements(GL_LINES, numberOfLines * 2, GL_UNSIGNED_SHORT, (void*)0);
    glLineWidth(currentLineWidth);
}

BoundingRect Actor::getBoundingRect() const {
    return BoundingRect(model->getBoundingRect(),transformModelViewPreRender(glm::mat4()));
}

BoundingSphere Actor::getBoundingSphere() const  {
    return (model->getBoundingSphere() + this->position) * scale;
}

glm::mat4 Actor::transformModelViewPreRender(const glm::mat4 & model) const {
    return model * glm::translate(this->position) * glm::orientate4(rotation) * glm::scale(glm::mat4(),glm::vec3(this->scale,this->scale,this->scale));
}

void Actor::setPosition(glm::vec3 position) {
    this->position = position;
}

const glm::vec3 &Actor::getPosition() const {
    return position;
}

void Actor::setRotation(glm::vec3 rotation) {
    this->rotation = rotation;
}

void Actor::accelerate(const glm::vec3 & vec) {
    accelerator->accelerate(vec);
}

void Actor::loadBoundingBox(const glm::vec3 & begin, const glm::vec3 & end, float radius, glm::vec3 * normals) {
    if (vbo != NULL) {
        glDeleteBuffers(2,vbo);
        delete vbo;
    }
    vbo = new GLuint[2];
    glm::vec3 middle = (begin + end) / 2.0f ;
    float pointArray[3*16] = {0};

    // 0
    int i = 0;
    pointArray[(i*3) + 0] = begin.x;
    pointArray[(i*3) + 1] = begin.y;
    pointArray[(i*3) + 2] = begin.z;

    // 1
    i++;
    pointArray[(i*3) + 0] = begin.x;
    pointArray[(i*3) + 1] = end.y;
    pointArray[(i*3) + 2] = begin.z;

    // 2
    i++;
    pointArray[(i*3) + 0] = end.x;
    pointArray[(i*3) + 1] = end.y;
    pointArray[(i*3) + 2] = begin.z;

    // 3
    i++;
    pointArray[(i*3) + 0] = end.x;
    pointArray[(i*3) + 1] = begin.y;
    pointArray[(i*3) + 2] = begin.z;

    // 4
    i++;
    pointArray[(i*3) + 0] = begin.x;
    pointArray[(i*3) + 1] = begin.y;
    pointArray[(i*3) + 2] = end.z;

    // 5
    i++;
    pointArray[(i*3) + 0] = begin.x;
    pointArray[(i*3) + 1] = end.y;
    pointArray[(i*3) + 2] = end.z;

    // 6
    i++;
    pointArray[(i*3) + 0] = end.x;
    pointArray[(i*3) + 1] = end.y;
    pointArray[(i*3) + 2] = end.z;

    // 7
    i++;
    pointArray[(i*3) + 0] = end.x;
    pointArray[(i*3) + 1] = begin.y;
    pointArray[(i*3) + 2] = end.z;

    // 8
    i++;
    pointArray[(i*3) + 0] = middle.x;
    pointArray[(i*3) + 1] = middle.y;
    pointArray[(i*3) + 2] = middle.z;

    // 9-14
    for (int currentNormal = 0; currentNormal < 6; currentNormal++ ) {
        i++;
        pointArray[(i*3) + 0] = middle.x+normals[currentNormal].x;
        pointArray[(i*3) + 1] = middle.y+normals[currentNormal].y;
        pointArray[(i*3) + 2] = middle.z+normals[currentNormal].z;
    }

    GLshort indeces[] = {
            0,1,
            1,2,
            2,3,
            3,0,
            4,5,
            5,6,
            6,7,
            7,4,
            0,4,
            1,5,
            2,6,
            3,7,

            // normals
            8,9,
            8,10,
            8,11,
            8,12,
            8,13,
            8,14
    };

    glGenBuffers(2, vbo);
    checkGlError("character->glGenBuffers");
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    checkGlError("character[GL_ARRAY_BUFFER]->glBindBuffer");
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat) * 15 * 3,(GLvoid *) pointArray, GL_STATIC_DRAW);
    checkGlError("character[GL_ARRAY_BUFFER]->glBufferData");
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    checkGlError("character->glBindBuffer");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 18 * 2,(GLvoid *) indeces, GL_STATIC_DRAW);
    checkGlError("character->glBufferData");
}

void Actor::setForces(const glm::vec3 & force) {
    accelerator->setForces(force);
}

void Actor::decelerate(float decellerationMagnitude) {
    accelerator->calculateRateOfDeceleration(decellerationMagnitude);
}

t_collision_model Actor::collisionModel() const {
    return collision_model;
}

void Actor::setCharacterEventId(int characterId) {
    characterEventId = characterId;
}

int32_t Actor::getId() const {
    return characterEventId;
}

void Actor::setScale(float scale) {
    this->scale = scale;
}

void Actor::serialize(lua_State *L) {
    create_lua_character(L,this);
}

const glm::vec3 & Actor::getVelocity() const {
    return accelerator->getVelocity();
}

float Actor::getMass() const {
    return mass;
}


