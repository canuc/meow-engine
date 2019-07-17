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

#ifndef NYAN_ANDROID_MD5_MODEL_H
#define NYAN_ANDROID_MD5_MODEL_H

#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <meow_gl.h>
#include "util/quat.h"
#include <sstream>
#include "md5_animation.h"

#include "imodel.h"

using namespace std;

class MD5Model: public IModel {
public:
    MD5Model();

    virtual ~MD5Model();

    bool loadMd5(const std::string &filename);

    bool loadAnim(const std::string &filename);

    void update(meow_time_t fDeltaTime);

    void draw(meow_time_t time, ShaderManager *shaderManager, const glm::mat4 &model,
                const glm::mat4 &view, const glm::mat4 &projectionView, const glm::vec3 &eyePos,
                const glm::vec3 &lookAt);

    struct Weight {
        int m_JointID;
        float m_Bias;
        glm::vec3 m_Pos;
    };

    BoundingRect getBoundingRect() const {
        return m_bounds;
    }

    BoundingSphere getBoundingSphere() const {
        return m_boundedSphere;
    }

protected:
    typedef std::vector<glm::vec3> PositionBuffer;
    typedef std::vector<glm::vec3> NormalBuffer;
    typedef std::vector<glm::vec2> Tex2DBuffer;
    typedef std::vector<GLushort> IndexBuffer;
    typedef std::vector<glm::vec4> WeightBuffer;
    typedef std::vector<glm::vec4> BoneIndexBuffer;

    struct Vertex {
        glm::vec3   m_Pos;
        glm::vec3   m_Normal;
        glm::vec3   m_Tangent;
        glm::vec3   m_Bitangent;
        glm::vec2   m_Tex0;
        glm::vec4   m_BoneWeights;
        glm::vec4   m_BoneIndices;

        int m_StartWeight;
        int m_WeightCount;
    };
    typedef std::vector<Vertex> VertexList;

    struct Triangle {
        int m_Indices[3];
    };
    typedef std::vector<Triangle> TriangleList;


    typedef std::vector<Weight> WeightList;

    struct Joint {
        std::string m_Name;
        int m_ParentID;
        glm::vec3 m_Pos;
        glm::quat m_Orient;
    };
    typedef std::vector<Joint> JointList;

    struct Mesh {
        std::string m_Shader;
        // This vertex list stores the vertices in the bind pose.
        VertexList m_Verts;
        TriangleList m_Tris;
        WeightList m_Weights;

        // A texture ID for the material
        GLuint m_TexID;

        // These buffers are used for rendering the animated mesh
        PositionBuffer m_PositionBuffer; // Vertex position stream
        NormalBuffer m_NormalBuffer;     // Vertex normals stream
        Tex2DBuffer m_Tex2DBuffer;       // Texture coordinate set
        IndexBuffer m_IndexBuffer;       // Vertex index buffer
        NormalBuffer m_BitangentBuffer;  // Vertex bittangent buffer used in bumpmapping
        NormalBuffer m_TangentBuffer;    // Vertex tangent buffer used in bumpmapping
        BoneIndexBuffer m_BoneIndex;     // Bone index
        WeightBuffer m_BoneWeights;      // Bone weights buffer

        GLuint positionBuffer;
        GLuint normalBuffer;
        GLuint textcoordBuffer;
        GLuint tangentBuffer;
        GLuint bitangentBuffer;
        GLuint boneWeights;
        GLuint boneIndex;
        GLuint indexBuffer;
    };

    typedef std::vector<Mesh> MeshList;

    bool checkAnimation(const MD5Animation &animation) const;

    // Prepare the mesh for rendering
    // Compute vertex positions and normals
    bool prepareMesh(Mesh & mesh);

    bool prepareNormals(Mesh &mesh);

    // render a single mesh of the model
    void renderMesh(const Mesh &mesh, meow_time_t time, ShaderManager *shaderManager, const glm::mat4 &model,
                    const glm::mat4 &view, const glm::mat4 &projectionView, const glm::vec3 &eyePos,
                    const glm::vec3 &lookAt);

    void initializeBuffers(Mesh & mesh);
    void releaseBuffers(Mesh & mesh);
    void BuildBindPose( const JointList& joints );
//    void RenderNormals( const Mesh& mesh );

    // Draw the skeleton of the mesh for debugging purposes.
//    void RenderSkeleton( const JointList& joints );

    // bool checkAnimation( const MD5Animation& animation ) const;
private:
    typedef std::vector<glm::mat4x4> MatrixList;

    int m_iMD5Version;
    int m_iNumJoints;
    int m_iNumMeshes;

    bool m_bHasAnimation;
    MatrixList m_AnimatedBones;

    JointList m_Joints;
    MeshList m_Meshes;
    MatrixList          m_BindPose;
    MatrixList          m_InverseBindPose;

    MD5Animation m_Animation;
    meow_time_t lastTime;

    glm::mat4x4 m_LocalToWorldMatrix;
    BoundingRect m_bounds;
    BoundingSphere m_boundedSphere;
};

#endif //NYAN_ANDROID_MD5_MODEL_H
