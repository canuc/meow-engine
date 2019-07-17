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

#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>
#include <vector>

using namespace std;
#include <lualibs/lua_serializable.h>
#include "inode.h"
#include "wavefrontobj.h"
#include "ibounded.h"
#include "accelerator.h"
#include "igravitable.h"
#include "texture_manager.h"
#include "shaders.h"
#include "icollidable.h"



#define TOKEN_CHARACTER_NAME "name"
#define TOKEN_CHARACTER_SHADER_VERTEX "vertex"
#define TOKEN_CHARACTER_SHADER_FRAGMENT "fragment"
#define TOKEN_CHARACTER_SHADER_TEXTURE "texture"
#define TOKEN_CHARACTER_BUMP_TEXTURE "bump"
#define TOKEN_CHARACTER_MODEL "model"
#define TOKEN_CHARACTER_SCALE "scale"
#define TOKEN_CHARACTER_ROTATE "rotate"
#define TOKEN_CHARACTER_BOUNDING "bounding"
#define TOKEN_CHARACTER_INCLUDE "include"
#define TOKEN_CHARACTER_UNIFORM "uniform"
#define TOKEN_CHARACTER_POSITION "position"
#define TOKEN_CHARACTER_MASS "mass"
#define TOKEN_CHARACTER_BLENDING "blending"
#define TOKEN_CHARACTER_COLLISION "collision_enabled"
#define TOKEN_CHARACTER_MD5_MODEL "md5_model"
#define TOKEN_CHARACTER_MD5_ANIM "md5_animation"

#define NODE_TYPE_ACTOR 1

struct ActorDesciptor {
    string name;
    string vertex;
    string fragment;
    string model;
    string texture;
    string bump;
    string md5model;
    string md5anim;
    float mass;
    float scale;
    bool circular;
    bool hasCircular;
    bool blending;
    bool hasBlending;
    bool hasCollision;
    bool collisionEnabled;
    glm::vec3 rotation;
    glm::vec3 position;
    std::map<string,float> uniforms;
};

typedef ActorDesciptor *PActorDescriptor;
class ModelManager;

class Actor : public IGravitable, public ILuaSerializable
{
public:
    virtual int32_t getId() const;

    Actor(const string & file, ModelManager * models, ShaderManager * shaders, TextureManager *textures, PActorDescriptor * overrideDescriptor = NULL);
    virtual ~Actor();

    /**
     * The boundings depending on the character model.
     */
    BoundingRect getBoundingRect() const;
    BoundingSphere getBoundingSphere() const;

    /**
     * These are accelerator mutation properties.
     */
    void accelerate(const glm::vec3 & vec);
    void decelerate(float decellerationMagnitude);
    void setForces(const glm::vec3 & force);
    void setScale(float scale);

    virtual void serialize(lua_State *L);
    /**
     * Actor Event Accessors
     */
    void setCharacterEventId(int characterId);

    void setPosition(glm::vec3 position);

    const glm::vec3 & getPosition() const;
    float getMass() const;
    const glm::vec3 & getVelocity() const;
    const string &getName() const;

    int getNodeType() const {
        return NODE_TYPE_ACTOR;
    }

    static PActorDescriptor createDescriptor();
protected:
    void processNode(meow_time_t time, engine *gameEngine);
    virtual t_collision_model collisionModel() const;

    PActorDescriptor loadCharacterDescriptor(const string & file);
    PActorDescriptor mergeCharacterDescriptor(PActorDescriptor characterDescriptor, PActorDescriptor subDescriptor);

    void loadCharacter(const string & file, PActorDescriptor * descriptor, ShaderManager * shader);
    GLuint loadTexture(const string & filename);

    void drawNode(meow_time_t time, ShaderManager * shaderManager, const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projectionView, const glm::vec3 & eyePos, const glm::vec3 &lookAt);

    virtual glm::mat4 transformModelViewPreRender(const glm::mat4 & modelView) const;
    virtual void setRotation(glm::vec3 rotation);

    bool collisionEnabled() const;
private:
    IModel * model;
    string name;
    GLuint texID;
    GLuint bumpID;
    GLint shaderID;
    GLfloat scale;
    glm::vec3 rotation;
    glm::vec3 position;
    Accelerator * accelerator;
    ShaderManager * shaderManager;
    ModelManager * modelManager;
    TextureManager *textureManager;
    bool hasTexture;
    bool hasBump;
    GLfloat mass;
    int characterEventId;
    bool hasBlending;
    bool isCollisionEnabled;
    string filename;
    t_collision_model collision_model;
    GLuint * vbo;
    GLint boundingShader;

    map<string,float> uniforms;
    void loadBoundingBox(const glm::vec3 &begin, const glm::vec3 &end, float radius, glm::vec3 * normals);

    void drawBoundingBox(meow_time_t time, ShaderManager *shaderManager, const glm::mat4 &model,
                         const glm::mat4 &view, const glm::mat4 &projectionView,
                         const glm::vec3 &eyePos);
};


#endif // CHARACTER_H
