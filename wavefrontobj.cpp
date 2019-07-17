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

#include "wavefrontobj.h"
#include <cstring>
#include <vector>
#include <sstream>
#include <cmath>
#include <map>
#include <glm/vec2.hpp>
#include <glm/geometric.hpp>
#include "platform_fopen.h"

#include "meow.h"
#include "meow_util.h"
#include "shaders.h"

#define TOKEN_VERTEX_POS "v"
#define TOKEN_VERTEX_NOR "vn"
#define TOKEN_VERTEX_TEX "vt"
#define TOKEN_FACE "f"
#define TOKEN_GROUP "g"
#define TOKEN_MTLLIB "mtllib"
#define TOKEN_USEMTL "usemtl"

#define TOKEN_MTL_NEW "newmtl"
#define TOKEN_MTL_AMBIENT "Ka"
#define TOKEN_MTL_DIFFUSE "Kd"
#define TOKEN_MTL_SPECULAR "Ks"
#define TOKEN_MTL_COEFF    "Ni"

#define OBJ_INT_BUFFER 20

// This should handle mtls of the format:
//newmtl Filling
//Ns 96.078431
//Ka 0.000000 0.000000 0.000000
//Kd 0.640000 0.244694 0.622469
//Ks 0.500000 0.500000 0.500000
//Ni 1.000000
//d 1.000000
//illum 2

#define APPNAME "RightMeow"
/*!
 * \brief The Vector3f struct
 * This is the only type struct that we will need
 */

struct ObjMeshVertex{
    int pos_index;
    int tex_index;
    int normal_index;
};

/* This is a triangle, that we can render */
struct ObjMeshFace{
    ObjMeshVertex vertices[3];
};

/* This contains a list of triangles */
struct ObjGroup {
    vector<ObjMeshFace> faces;
    // Add mtl
    string mtlgroup;
};

struct mtl {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float     specularcoefficient;
};

struct vertex_attribs {
    GLfloat pos[3];
    GLfloat diffuse[3];
    GLfloat normals[3];
    GLfloat texture[2];
    GLfloat tangent[3];
    GLfloat bitangent[3];
};

static size_t getBufferSize(const vector<ObjGroup> * checkBuffer);
#ifdef USE_ANDROID
WavefrontObj::WavefrontObj(struct engine * app_engine, const string & file)
{
    hasTexture = false;
    this->app_engine = app_engine;
    this->filename = file;
    loadObj(file);
}
#else
WavefrontObj::WavefrontObj(const string & file)
{
    hasTexture = false;
    loadObj(file);
}
#endif


WavefrontObj::~WavefrontObj() {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &indexBuffer);
}

void WavefrontObj::loadMtl(const string & file,map<string,mtl> & mtls)
{
    char  buffer[LINE_BUFFER_SIZE] = {0};
    string command, currentmtlname;
    int readSentinel = 0;
    bool firstMaterial = true;
    ALOGI("Found material lib: %s\n", file.c_str());

    FILE * rwOps = meow_fopen(file.c_str(),"r");

    if ( rwOps != NULL ) {
        memset(buffer, 0x00, LINE_BUFFER_SIZE);
        readSentinel = readline(rwOps, buffer);
        mtl currentMaterialBuffer;

        while ( readSentinel != EOF_SENTINEL ) {
            stringstream stringReadStream(buffer);
            stringReadStream >> command;
#ifdef DEBUG_WAVEFRONT
            ALOGI("Command: %s\n", command.c_str());
#endif
            if ( command == TOKEN_MTL_NEW ) {
                if ( !firstMaterial ) {
                    mtls[currentmtlname] = currentMaterialBuffer;
                }

                stringReadStream >> currentmtlname;
                currentMaterialBuffer = mtl();
                firstMaterial = false;
            } else if ( command == TOKEN_MTL_AMBIENT ) {
                stringReadStream >> currentMaterialBuffer.ambient.x >> currentMaterialBuffer.ambient.y >> currentMaterialBuffer.ambient.z;
            } else if ( command == TOKEN_MTL_DIFFUSE ) {
                stringReadStream >> currentMaterialBuffer.diffuse.x >> currentMaterialBuffer.diffuse.y >> currentMaterialBuffer.diffuse.z;
            } else if ( command == TOKEN_MTL_SPECULAR ) {
                stringReadStream >> currentMaterialBuffer.specular.x >> currentMaterialBuffer.specular.y >> currentMaterialBuffer.specular.z;
            } else if ( command == TOKEN_MTL_COEFF ){
                stringReadStream >> currentMaterialBuffer.specularcoefficient;
            }

            memset(buffer, 0x00, LINE_BUFFER_SIZE);
            readSentinel = readline(rwOps,buffer);
        }

        if ( !firstMaterial ) {
            mtls[currentmtlname] = currentMaterialBuffer;
        }

        // release the file handle
        fclose(rwOps);
    }
#ifdef DEBUG_WAVEFRONT
    ALOGE("number of materials: %lu\n", mtls.size());
#endif
}

static size_t getBufferSize(const vector<ObjGroup> * checkBuffer)
{
    size_t buffer_size = 0;

    for ( size_t i = 0; i < checkBuffer->size(); i++ ) {
        // Stride = sizeof(xyz) + sizeof(diffuse) + sizeof(normal)
        buffer_size += checkBuffer->at(i).faces.size() * 3;
    }

    return buffer_size;
}

void WavefrontObj::draw(meow_time_t time, ShaderManager *shaderManager, const glm::mat4 &model,
          const glm::mat4 &view, const glm::mat4 &projectionView, const glm::vec3 &eyePos,
          const glm::vec3 &lookAt) {
    checkGlError("preDraw");

    if (shaderManager->hasUniform(shaderID,SHADER_UNIFORM_MODEL_MATRIX)) {
        // Bind the modelview & projection
        glUniformMatrix4fv(shaderManager->getUniform(shaderID, SHADER_UNIFORM_MODEL_MATRIX), 1, GL_FALSE,
                           glm::value_ptr(model));
        checkGlError(SHADER_UNIFORM_MODEL_MATRIX);
    } else {
        ALOGE("ERROR no model matrix uniform! %s", filename.c_str());
    }

    if (shaderManager->hasUniform(shaderID,SHADER_UNIFORM_VIEW_MATRIX)) {
        // Bind the modelview & projection
        glUniformMatrix4fv(shaderManager->getUniform(shaderID, SHADER_UNIFORM_VIEW_MATRIX), 1, GL_FALSE,
                           glm::value_ptr(view));
        checkGlError(SHADER_UNIFORM_VIEW_MATRIX);
    } else {
        ALOGE("ERROR no view matrix uniform! %s", filename.c_str());
    }

    if ( shaderManager->hasUniform(shaderID,SHADER_UNIFORM_PROJECTION_MATRIX)) {
        glUniformMatrix4fv(shaderManager->getUniform(shaderID, SHADER_UNIFORM_PROJECTION_MATRIX), 1, GL_FALSE,
                           glm::value_ptr(projectionView));
        checkGlError(SHADER_UNIFORM_PROJECTION_MATRIX);
    } else {
        ALOGE("ERROR no perspective matrix uniform! %s", filename.c_str());
    }

    if (shaderManager->hasUniform(shaderID, SHADER_UNIFORM_EYE_VEC)) {
        GLuint shaderEyeVec = shaderManager->getUniform(shaderID,SHADER_UNIFORM_EYE_VEC);
        glUniform3fv(shaderEyeVec, 1, glm::value_ptr(eyePos));
        checkGlError("[u_eyeVec]glUniform3fv");
    }

    if (shaderManager->hasUniform(shaderID, SHADER_UNIFORM_TIME)) {
        GLfloat timeToShader = time;
        GLuint shaderUniformTime = shaderManager->getUniform(shaderID,SHADER_UNIFORM_TIME);
        glUniform1f(shaderUniformTime,timeToShader);
        checkGlError("[u_time]glUniform1i");
    }

    // get the attributes
    GLuint attribPosition = shaderManager->getAttrib(shaderID,SHADER_ATTRIBUTE_POSITION);
    GLuint attribColor    = shaderManager->getAttrib(shaderID,SHADER_ATTRIBUTE_COLOR);
    GLuint attribTexture  = shaderManager->getAttrib(shaderID,SHADER_ATTRIBUTE_TEX_COORD);
    GLuint attribNormal   = shaderManager->getAttrib(shaderID,SHADER_ATTRIBUTE_NORMAL);
    GLuint attribBitangent= shaderManager->getAttrib(shaderID,SHADER_ATTRIBUTE_BITANGENT);
    GLuint attribTangent  = shaderManager->getAttrib(shaderID,SHADER_ATTRIBUTE_TANGENT);

    if (hasTexture && shaderManager->hasUniform(shaderID,SHADER_UNIFORM_TEXTURE_ID)) {
        glActiveTexture(GL_TEXTURE0);
        checkGlError("glActiveTexture");
        glBindTexture(GL_TEXTURE_2D, textureID);
        checkGlError("glBindTexture");
        glUniform1i(shaderManager->getUniform(shaderID,SHADER_UNIFORM_TEXTURE_ID),0);
        checkGlError("glBindTexture");
    }

    if (hasBump) {
        glActiveTexture(GL_TEXTURE1);
        checkGlError("glActiveTexture");
        glBindTexture(GL_TEXTURE_2D, bumpID);
        checkGlError("glBindTexture");
        glUniform1i(shaderManager->getUniform(shaderID,SHADER_UNIFORM_BUMP_ID),1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    checkGlError("glBindBuffer");

    if (shaderManager->hasAttrib(shaderID, SHADER_ATTRIBUTE_POSITION)) {
        glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_attribs), (GLvoid * ) offsetof(struct vertex_attribs,pos));
        checkGlError("attribPosition->glVertexAttribPointer");
        glEnableVertexAttribArray(attribPosition);
        checkGlError("attribPosition->glEnableVertexAttribArray");
    }

    if (shaderManager->hasAttrib(shaderID, SHADER_ATTRIBUTE_COLOR)) {
        glEnableVertexAttribArray(attribColor);
        checkGlError("attribColor->glEnableVertexAttribArray");
        glVertexAttribPointer(attribColor, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_attribs),(GLvoid * ) offsetof(struct vertex_attribs,diffuse));
        checkGlError("attribColor->glEnableVertexAttribArray");
    }

    if (shaderManager->hasAttrib(shaderID, SHADER_ATTRIBUTE_TEX_COORD)) {
        glEnableVertexAttribArray(attribTexture);
        checkGlError("attribTexture->glEnableVertexAttribArray");
        glVertexAttribPointer(attribTexture, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_attribs),(GLvoid * ) offsetof(struct vertex_attribs,texture));
        checkGlError("attribTexture->glEnableVertexAttribArray");
    }

    if (shaderManager->hasAttrib(shaderID,SHADER_ATTRIBUTE_NORMAL)) {
        glEnableVertexAttribArray(attribNormal);
        checkGlError("[normals]attribNormal->glEnableVertexAttribArray");
        glVertexAttribPointer(attribNormal, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_attribs),
                              (GLvoid * ) offsetof(struct vertex_attribs,normals));
        checkGlError("[normals]attribNormal->glVertexAttribPointer");
    }

    if (shaderManager->hasAttrib(shaderID,SHADER_ATTRIBUTE_BITANGENT)) {
        glEnableVertexAttribArray(attribBitangent);
        checkGlError("[normals]attribBitangent->glEnableVertexAttribArray");
        glVertexAttribPointer(attribBitangent, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_attribs),
                              (GLvoid * ) offsetof(struct vertex_attribs,bitangent));
        checkGlError("[normals]attribBitangent->glVertexAttribPointer");
    }

    if (shaderManager->hasAttrib(shaderID,SHADER_ATTRIBUTE_TANGENT)) {
        glEnableVertexAttribArray(attribTangent);
        checkGlError("[normals]attribTangent->glEnableVertexAttribArray");
        glVertexAttribPointer(attribTangent, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_attribs),
                              (GLvoid * ) offsetof(struct vertex_attribs,tangent));
        checkGlError("[normals]attribTangent->glVertexAttribPointer");
    }

    // Bind our VBO

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    checkGlError("glBindBuffer");
    glDrawElements(GL_TRIANGLES, numberOfVertecies , GL_UNSIGNED_SHORT, (void*)0);
    checkGlError("glDrawElements");

    clearBoundBuffers();
}

void WavefrontObj::loadObj(const string & file) {
    //SDL_RWops * rwOps = SDL_RWFromFile(file.c_str(),"r");
    char  buffer[LINE_BUFFER_SIZE] = {0};
    int eofsentinel = 0;
    size_t relativeFilePosition = file.find_last_of('/');
    string relativeFilePath = "";

    if (relativeFilePosition != string::npos) {
        relativeFilePath = file.substr(0, relativeFilePosition+1);
    }
    glm::vec3 top;
    glm::vec3 bottom;

    vector<glm::vec3> pospool;           // pool of all positions refrenced by index, in each a face
    vector<glm::vec2> texPool;           // p   ool of all the textures
    vector<glm::vec3> normpool;          // pool of normals refrenced by index, in each face
    vector<ObjGroup> objectgroupvecs;    // Object groups contain a list of faces and a material to bind to
    map<string,mtl> mtls;                // A map of materials, that is handled by \ref WavefrontObj::loadMtl


    FILE * rwOps = meow_fopen(file.c_str(),"r");

    if ( rwOps != NULL ) {
        // Zero out all the memory
        memset(buffer, 0x00, LINE_BUFFER_SIZE);
        ObjGroup currObjGroup;
        eofsentinel = readline(rwOps,buffer);
        string commandBuffer;

        while( eofsentinel != EOF_SENTINEL ) {

            stringstream stringInputStream(buffer);
            stringInputStream >> commandBuffer;

             if ( commandBuffer == TOKEN_VERTEX_POS ) {
                glm::vec3 currentVector;
                stringInputStream >> currentVector.x >> currentVector.y >> currentVector.z;
                pospool.push_back(currentVector);
             } else if ( commandBuffer == TOKEN_VERTEX_TEX) {
                glm::vec2 currentTexture;
                stringInputStream >> currentTexture.x >> currentTexture.y;
                texPool.push_back(currentTexture);
             } else if ( commandBuffer == TOKEN_VERTEX_NOR ) {
                glm::vec3 currentVector;
                stringInputStream >> currentVector.x >> currentVector.y >> currentVector.z;
                glm::vec3 normalizedVec = glm::normalize(currentVector);

                // Okay the vector is normalized
                normpool.push_back(normalizedVec);
             } else if ( commandBuffer == TOKEN_FACE ) {
                string currface;
                char currentPoint[OBJ_INT_BUFFER] = {0};

                ObjMeshFace objFaceMesh;

                getline(stringInputStream,currface,' ');

                for ( int i = 0; i < 3; i++ ) {
                    getline(stringInputStream,currface,' ');
                    memset(currentPoint,0x00, OBJ_INT_BUFFER);
                    memcpy(currentPoint,currface.c_str(),currface.size());

                    char * extracted = currentPoint;
                    int vertexInfoIndex = 0;
                    int currentInt = 0;

                    objFaceMesh.vertices[i].normal_index = -1;
                    objFaceMesh.vertices[i].pos_index = -1;
                    objFaceMesh.vertices[i].tex_index = -1;
                    char * token = NULL;
                    while ((token = strsep(&extracted,"/"))) {
                        if (strlen(token) > 0) {
                            currentInt = atoi(token) - 1;

                            switch (vertexInfoIndex) {
                                case 0:
                                    objFaceMesh.vertices[i].pos_index = currentInt;
                                    break;
                                case 1:
                                    objFaceMesh.vertices[i].tex_index = currentInt;
                                    break;
                                case 2:
                                    objFaceMesh.vertices[i].normal_index = currentInt;
                                    break;
                            }
                        }
                        vertexInfoIndex++;
                    }

                    if ( objFaceMesh.vertices[i].pos_index < 0 ) {
                        ALOGE("invalid vertecies: %d at %d",objFaceMesh.vertices[i].pos_index, i);
                    }

                    if ( objFaceMesh.vertices[i].normal_index < 0 ) {
                        ALOGE("invalid normal: %d at %d",objFaceMesh.vertices[i].normal_index, i);
                    }
                }

                // We have a face, so append it to the current object group
                currObjGroup.faces.push_back(objFaceMesh);
             } else if ( commandBuffer == TOKEN_GROUP ) {
                 if ( currObjGroup.faces.size() ) {
                    // if it contains some vertex information
                    // push it onto the vector, and flush the face buffer
                    objectgroupvecs.push_back(currObjGroup);
                 }
                currObjGroup = ObjGroup();
             } else if ( commandBuffer == TOKEN_MTLLIB ) {
#ifdef DEBUG_WAVEFRONT
                 ALOGI("Logging output relative path: %s\n",relativeFilePath.c_str());
#endif
                 string mtllibfilename;
                stringInputStream >> mtllibfilename;
                loadMtl(relativeFilePath+mtllibfilename,mtls);
             } else if ( commandBuffer == TOKEN_USEMTL ) {
                 string materialName;
                 stringInputStream >> materialName;
                 currObjGroup.mtlgroup = materialName;
             }

            memset(buffer, 0x00, LINE_BUFFER_SIZE);
            eofsentinel = readline(rwOps,buffer);
        }

        // Stride = sizeof(x)+sizeof(y)+sizeof(z) + sizeof(diffuse) + sizeof(normal)
         if ( currObjGroup.faces.size() ) {
            // if it contains some vertex information
            // push it onto the vector, and flush the face buffer
            objectgroupvecs.push_back(currObjGroup);
         }

         // release the file handle
         fclose(rwOps);
    } else {
        ALOGE("Error with loading wavefront obj: %s",file.c_str());
        return;
    }

    size_t bufferSize = getBufferSize(&objectgroupvecs);
    // now translate those faces to attrib arrays with the correct components
    // Stride = sizeof(x)+sizeof(y)+sizeof(z) + sizeof(diffuse) + sizeof(normal)
    vertex_attribs * vertices = new vertex_attribs[bufferSize];
    size_t currentVerteces = 0;
    numberOfVertecies = 0;
    glm::vec3 face_verts[3];
    glm::vec2 face_uvs[2];

    for ( size_t i = 0; i < objectgroupvecs.size(); i++ ) {
        mtl currentMtl = mtls[objectgroupvecs[i].mtlgroup];

        for ( size_t inner = 0; inner < objectgroupvecs[i].faces.size(); inner++) {
            size_t beforeStartVerts = currentVerteces;

            for ( size_t vertex = 0; vertex < 3; vertex++ ) {

                if (objectgroupvecs[i].faces[inner].vertices[vertex].pos_index < 0) {
                    ALOGE("Error with vertex in: group id: %lu face: %lu vertex: %lu\n", i, inner,vertex);
                }

                const glm::vec3 & positionVertex = pospool[objectgroupvecs[i].faces[inner].vertices[vertex].pos_index];
                vertices[currentVerteces].pos[0] = positionVertex.x;
                vertices[currentVerteces].pos[1] = positionVertex.y;
                vertices[currentVerteces].pos[2] = positionVertex.z;

                if ( positionVertex.x > top.x ) {
                    top.x = positionVertex.x;
                }

                if ( positionVertex.x < bottom.x ) {
                    bottom.x = positionVertex.x;
                }

                if ( positionVertex.y > top.y ) {
                    top.y = positionVertex.y;
                }

                if (positionVertex.y < bottom.y ) {
                    bottom.y = positionVertex.y;
                }

                if (positionVertex.z > top.z ) {
                    top.z = positionVertex.z;
                }

                if (positionVertex.z < bottom.z) {
                    bottom.z = positionVertex.z;
                }

                vertices[currentVerteces].diffuse[0] = currentMtl.diffuse.x;
                vertices[currentVerteces].diffuse[1] = currentMtl.diffuse.y;
                vertices[currentVerteces].diffuse[2] = currentMtl.diffuse.z;

                if (objectgroupvecs[i].faces[inner].vertices[vertex].tex_index < 0) {
                    // ALOGE("Error with texture coord in: group id: %lu face: %lu vertex: %lu\n",i,inner,vertex);
                    vertices[currentVerteces].texture[0] = 0;
                    vertices[currentVerteces].texture[1] = 0;
                } else {
                    vertices[currentVerteces].texture[0] = texPool[objectgroupvecs[i].faces[inner].vertices[vertex].tex_index].x;
                    vertices[currentVerteces].texture[1] = 1.0f - texPool[objectgroupvecs[i].faces[inner].vertices[vertex].tex_index].y;
                }

                if (objectgroupvecs[i].faces[inner].vertices[vertex].normal_index < 0) {
                    // ALOGE("Error with texture coord in: group id: %lu face: %lu vertex: %lu\n",i,inner,vertex);
                    vertices[currentVerteces].normals[0] = 0;
                    vertices[currentVerteces].normals[1] = 0;
                    vertices[currentVerteces].normals[2] = 0;
                } else {
                    vertices[currentVerteces].normals[0] = normpool[objectgroupvecs[i].faces[inner].vertices[vertex].normal_index].x;
                    vertices[currentVerteces].normals[1] = normpool[objectgroupvecs[i].faces[inner].vertices[vertex].normal_index].y;
                    vertices[currentVerteces].normals[2] = normpool[objectgroupvecs[i].faces[inner].vertices[vertex].normal_index].z;
                }

                face_verts[vertex] = positionVertex;
                face_uvs[vertex] = glm::vec2(vertices[currentVerteces].texture[0],vertices[currentVerteces].texture[1]);

                currentVerteces++;
            }

            glm::vec3 deltaPos1 = glm::vec3(face_verts[1]) - glm::vec3(face_verts[0]);
            glm::vec3 deltaPos2 = glm::vec3(face_verts[2]) - glm::vec3(face_verts[0]);

            glm::vec2 deltaUV1 = face_uvs[1]-face_uvs[0];
            glm::vec2 deltaUV2 = face_uvs[2]-face_uvs[0];

            float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
            glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
            glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

            // All the verts for the face should have tangent space populated
            // with the correct bitangent and tangent
            while (beforeStartVerts < currentVerteces) {
                vertices[beforeStartVerts].bitangent[0] = bitangent.x;
                vertices[beforeStartVerts].bitangent[1] = bitangent.y;
                vertices[beforeStartVerts].bitangent[2] = bitangent.z;

                vertices[beforeStartVerts].tangent[0] = tangent.x;
                vertices[beforeStartVerts].tangent[1] = tangent.y;
                vertices[beforeStartVerts].tangent[2] = tangent.z;

                beforeStartVerts++;
            }
        }
    }

    numberOfVertecies = currentVerteces;

    GLushort * shortBuffer = new GLushort[numberOfVertecies];
    for ( size_t i = 0; i < numberOfVertecies; i++ ) {
        shortBuffer[i]= (GLushort) i;
    }

    ALOGI("Number of verteces: %lu, pos: %d\n",numberOfVertecies, (int) offsetof(struct vertex_attribs,pos));
    glGenBuffers(1, &vbo);
    checkGlError("glGenBuffers[vbo]");
    ALOGI("Createing buffer: %d %lu\n", numberOfVertecies, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertex_attribs) * numberOfVertecies,(GLvoid *) vertices, GL_STATIC_DRAW);
    delete [] vertices;


    glGenBuffers(1, &indexBuffer);
    checkGlError("glGenBuffers[indexBuffer]");
    ALOGI("index buffer: %d\n", indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLushort) * numberOfVertecies,(GLvoid *) shortBuffer, GL_STATIC_DRAW);
    delete [] shortBuffer;

    m_bounds = BoundingRect(top,bottom);
    m_boundedSphere = createBoundingSphere(m_bounds);
}

BoundingRect WavefrontObj::getBoundingRect() const {
    return m_bounds;
}

BoundingSphere WavefrontObj::getBoundingSphere() const {
    return m_boundedSphere;
}

