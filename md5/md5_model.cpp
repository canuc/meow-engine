//
// Created by julian on 4/17/16.
//

#include "md5/md5_model.h"
#include "meow.h"
#include "shader_manager.h"
#include "shaders.h"
#include "logging.h"
#include <algorithm>
#include <cctype>

const char * trimWhitespaceBuffer(const char * strBuffer, size_t strLength) {
    int currentTraversal = 0;

    while(isspace((unsigned char)*strBuffer) && currentTraversal++ < strLength) strBuffer++;

    return strBuffer;
}

MD5Model::MD5Model():
        m_iMD5Version(-1),
        m_iNumJoints(0),
        m_iNumMeshes(0),
        m_bHasAnimation(false),
        lastTime(0),
        m_LocalToWorldMatrix(1) {
}

MD5Model::~MD5Model() {
    for (int i = 0; i < this->m_Meshes.size(); i++) {
        releaseBuffers(m_Meshes[i]);
    }
}

bool MD5Model::loadMd5( const std::string &filename ) {
    FILE * file = meow_fopen(filename.c_str(),"r");
    char  md5Linebuffer[LINE_BUFFER_SIZE] = {0};
    int i = 0;

    if (file == NULL) {
        ALOGI("Error loading model: %s, cannot read file!",filename.c_str());
        return false;
    }

    fseek(file, 0L, SEEK_END);
    int fileLength = ftell(file);
    fseek(file, 0L, SEEK_SET);

    assert( fileLength > 0 );

    while ( ! feof(file) )
    {
        fgets (md5Linebuffer, sizeof (md5Linebuffer), file);
        const char * commandBufferTrimmed = trimWhitespaceBuffer(md5Linebuffer, strlen(md5Linebuffer));
        if (sscanf (commandBufferTrimmed, "MD5Version %d", &m_iMD5Version) == 1) {
            if (m_iMD5Version != 10) {
                ALOGI("MD5 Version is incorrect. Required Version is 10. currently: %d", m_iMD5Version);
                goto clean_up_file;
            }
        } else if (sscanf (commandBufferTrimmed, "numJoints %d", &m_iNumJoints) == 1) {
            if (m_iNumJoints <= 0 ) {
                ALOGI("Got %d joints!", m_iNumJoints);
                goto clean_up_file;
            } else {
                m_Joints.reserve(m_iNumJoints);
                m_AnimatedBones.assign( m_iNumJoints, glm::mat4x4(1.0f) );
            }
        } else if (sscanf (commandBufferTrimmed, "numMeshes %d", &m_iNumMeshes) == 1) {
            if (m_iNumMeshes <= 0) {
                ALOGI("Got %d meshes!", m_iNumMeshes);
                goto clean_up_file;
            } else {
                m_Meshes.reserve(m_iNumMeshes);
            }
        } else if (strncmp (commandBufferTrimmed, "joints {", 8) == 0) {
            /* Read each joint */
            for (i = 0; i < m_iNumJoints; ++i)
            {
                Joint joint;
                char jointBufferName[LINE_BUFFER_SIZE] = {0};
                /* Read whole line */
                fgets (md5Linebuffer, sizeof (md5Linebuffer), file);
                const char * currentMd5LineBuffer = trimWhitespaceBuffer(md5Linebuffer, strlen(md5Linebuffer));

                if (sscanf (currentMd5LineBuffer, "\"%[^\"]\" %d ( %f %f %f ) ( %f %f %f )",
                            jointBufferName, &joint.m_ParentID, &joint.m_Pos[0],
                            &joint.m_Pos[1], &joint.m_Pos[2], &joint.m_Orient[0],
                            &joint.m_Orient[1], &joint.m_Orient[2]) == 8)
                {
                    /* Compute the w component */
                    ComputeQuatW (joint.m_Orient);
                    joint.m_Name = jointBufferName;
                    m_Joints.push_back(joint);
                }
            }

            BuildBindPose( m_Joints );
        } else if (strncmp (commandBufferTrimmed, "mesh {", 6) == 0) {
            int vert_index = 0;
            int tri_index = 0;
            int weight_index = 0;
            float fdata[4];
            int idata[3];
            Mesh mesh;

            int numVerts, numTris, numWeights;
            while ( md5Linebuffer[0] != '}' && !feof(file) )  // Read until we get to the '}' character
            {
                fgets(md5Linebuffer,sizeof (md5Linebuffer),file);
                const char * currentMd5LineBuffer = trimWhitespaceBuffer(md5Linebuffer, strlen(md5Linebuffer));

                if (strstr (currentMd5LineBuffer, "shader ")) {
                    // TODO: load correct shader
                } else if (sscanf (currentMd5LineBuffer, "numverts %d", &numVerts) == 1) {
                    if (numVerts > 0) {
                        mesh.m_Verts.reserve(numVerts);
                    } else {
                        ALOGI("Error specified invalid vert number: %d",numVerts);
                    }
                } else if (sscanf (currentMd5LineBuffer, "numtris %d", &numTris) == 1) {
                    if (numTris > 0) {
                        mesh.m_Tris.reserve(numTris);
                    } else {
                        ALOGI("Error specified invalid number of tris: %d",numTris);
                    }
                } else if (sscanf (currentMd5LineBuffer, "numweights %d", &numWeights) == 1) {
                    if (numTris > 0) {
                        mesh.m_Weights.reserve(numWeights);
                    } else {
                        ALOGI("Error specified invalid number of weights: %d",numWeights);
                    }
                } else if (sscanf (currentMd5LineBuffer, "vert %d ( %f %f ) %d %d", &vert_index,
                                   &fdata[0], &fdata[1], &idata[0], &idata[1]) == 5) {
                    Vertex vert;
                    vert.m_Tex0.x = fdata[0];
                    vert.m_Tex0.y = fdata[1];
                    vert.m_StartWeight = idata[0];
                    vert.m_WeightCount = idata[1];

                    mesh.m_Verts.push_back(vert);
                }  else if (sscanf (currentMd5LineBuffer, "tri %d %d %d %d", &tri_index,
                                    &idata[0], &idata[1], &idata[2]) == 4) {
                    Triangle triangle;
                    triangle.m_Indices[0] = idata[0];
                    triangle.m_Indices[1] = idata[1];
                    triangle.m_Indices[2] = idata[2];
                    mesh.m_Tris.push_back(triangle);

                    mesh.m_IndexBuffer.push_back( (GLushort)idata[0] );
                    mesh.m_IndexBuffer.push_back( (GLushort)idata[1] );
                    mesh.m_IndexBuffer.push_back( (GLushort)idata[2] );

                }else if (sscanf (currentMd5LineBuffer, "weight %d %d %f ( %f %f %f )",
                                  &weight_index, &idata[0], &fdata[3],
                                  &fdata[0], &fdata[1], &fdata[2]) == 6)
                {
                    Weight weight;
                    weight.m_JointID  = idata[0];
                    weight.m_Bias     = fdata[3];
                    weight.m_Pos.x    = fdata[0];
                    weight.m_Pos.y    = fdata[1];
                    weight.m_Pos.z    = fdata[2];

                    mesh.m_Weights.push_back(weight);
                }
            }

            prepareMesh(mesh);
            prepareNormals(mesh);
            initializeBuffers(mesh);

            m_Meshes.push_back(mesh);
        }
    }

    ALOGI("Got number of joint: %d and actual number of verts: %d",m_iNumJoints,m_Joints.size());
    assert( m_Joints.size() == m_iNumJoints );
    assert( m_Meshes.size() == m_iNumMeshes );

    clean_up_file:
        fclose(file);
    return true;
}

bool MD5Model::prepareNormals(Mesh &mesh) {
    mesh.m_NormalBuffer.clear();
    mesh.m_TangentBuffer.clear();
    mesh.m_BitangentBuffer.clear();

    // Loop through all triangles and calculate the normal of each triangle
    for ( unsigned int i = 0; i < mesh.m_Tris.size(); ++i )
    {
        glm::vec3 v0 = mesh.m_Verts[ mesh.m_Tris[i].m_Indices[0] ].m_Pos;
        glm::vec3 v1 = mesh.m_Verts[ mesh.m_Tris[i].m_Indices[1] ].m_Pos;
        glm::vec3 v2 = mesh.m_Verts[ mesh.m_Tris[i].m_Indices[2] ].m_Pos;
        //compute 2 edges for the vertex space computing (normal, tangent, bitangent)
        glm::vec3 e0 = v2-v0, e1 = v1-v0;
        //Compute the normal
        glm::vec3 tmpNormal = glm::cross( e0, e1 );

        /*Now we compute the coefficients of the texture, to have a proportionnality
        (and so this will be smooth with the others vertices)*/
        float deltaT0 = mesh.m_Verts[ mesh.m_Tris[i].m_Indices[1] ].m_Tex0.y -
                        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[0] ].m_Tex0.y ;
        float deltaT1 = mesh.m_Verts[ mesh.m_Tris[i].m_Indices[2] ].m_Tex0.y -
                        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[0] ].m_Tex0.y;
        // the same with the bitangent
        float deltaB0 = mesh.m_Verts[ mesh.m_Tris[i].m_Indices[1] ].m_Tex0.x -
                        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[0] ].m_Tex0.x ;
        float deltaB1 = mesh.m_Verts[ mesh.m_Tris[i].m_Indices[2] ].m_Tex0.x -
                        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[0] ].m_Tex0.x;
        // le facteur permettant de rendre les tangentes et binormales
        // proportionnelles aux coordonnees de textures.
        // nous aurons donc des vecteurs dont la norme depend des coordonnees
        // de textures.
        float scale = 1/ ((deltaB0 * deltaT1) - (deltaB1 * deltaT0));

        // on calcule la tangente temporaire
        glm::vec3 tmpTangent = ((e0*deltaT1) - (e1*deltaT0))*scale;
        tmpTangent = glm::normalize(tmpTangent);

        // on calcule la binormale temporaire
        glm::vec3 tmpBitangent = ((e0*(-deltaB1)) + (e1*deltaB0))*scale;
        tmpBitangent = glm::normalize(tmpBitangent);

        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[0] ].m_Normal += tmpNormal;
        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[1] ].m_Normal += tmpNormal;
        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[2] ].m_Normal += tmpNormal;

        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[0] ].m_Tangent += tmpTangent;
        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[1] ].m_Tangent += tmpTangent;
        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[2] ].m_Tangent += tmpTangent;

        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[0] ].m_Bitangent += tmpBitangent;
        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[1] ].m_Bitangent += tmpBitangent;
        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[2] ].m_Bitangent += tmpBitangent;
    }

    // Now normalize all theses vectors
    for ( unsigned int i = 0; i < mesh.m_Verts.size(); ++i )
    {
        Vertex& vert = mesh.m_Verts[i];

        vert.m_Normal = glm::normalize( vert.m_Normal );
        vert.m_Tangent = glm::normalize( vert.m_Tangent );
        vert.m_Bitangent = glm::normalize( vert.m_Bitangent );
        mesh.m_NormalBuffer.push_back( vert.m_Normal );
        mesh.m_TangentBuffer.push_back( vert.m_Tangent );
        mesh.m_BitangentBuffer.push_back( vert.m_Bitangent );
    }

    return true;
}

bool MD5Model::loadAnim( const std::string& filename )
{
    if ( m_Animation.LoadAnimation( filename ) )
    {
        // Check to make sure the animation is appropriate for this model
        m_bHasAnimation = checkAnimation(m_Animation);
    }

    return m_bHasAnimation;
}

bool MD5Model::checkAnimation(const MD5Animation &animation) const
{
    if ( m_iNumJoints != animation.GetNumJoints() )
    {
        return false;
    }

    // Check to make sure the joints match up
    for ( unsigned int i = 0; i < m_Joints.size(); ++i )
    {
        const Joint& meshJoint = m_Joints[i];
        const MD5Animation::JointInfo& animJoint = animation.GetJointInfo( i );

        if ( meshJoint.m_Name != animJoint.m_Name ||
             meshJoint.m_ParentID != animJoint.m_ParentID )
        {
            return false;
        }
    }

    return true;
}

//bool MD5Model::PrepareMesh(Mesh & mesh, const std::vector<glm::mat4x4>& skel )
//{
//    for ( unsigned int i = 0; i < mesh.m_Verts.size(); ++i )
//    {
//        const Vertex & vert = mesh.m_Verts[i];
//        glm::vec3& pos = mesh.m_PositionBuffer[i];
//        glm::vec3& normal = mesh.m_NormalBuffer[i];
//
//        pos = glm::vec3(0);
//        normal = glm::vec3(0);
//
//        for ( int j = 0; j < vert.m_WeightCount; ++j )
//        {
//            const Weight& weight = mesh.m_Weights[vert.m_StartWeight + j];
//            const glm::mat4x4 boneMatrix = skel[weight.m_JointID];
//
//            pos += glm::vec3( ( boneMatrix * glm::vec4( vert.m_Pos, 1.0f ) ) * weight.m_Bias );
//            normal += glm::vec3( ( boneMatrix * glm::vec4( vert.m_Normal, 0.0f ) ) * weight.m_Bias );
//        }
//    }
//    return true;
//}

bool sortWeights (const pair<int, MD5Model::Weight> & i,const pair<int, MD5Model::Weight> & j) {
    return (i.second.m_Bias > j.second.m_Bias);
}

bool MD5Model::prepareMesh(Mesh &mesh) {

    mesh.m_PositionBuffer.clear();
    mesh.m_Tex2DBuffer.clear();
    mesh.m_BoneIndex.clear();
    mesh.m_BoneWeights.clear();

    // Compute vertex positions
    for ( unsigned int i = 0; i < mesh.m_Verts.size(); ++i )
    {
        glm::vec3 finalPos(0);
        Vertex& vert = mesh.m_Verts[i];

        vert.m_Pos = glm::vec3(0);
        vert.m_Normal = glm::vec3(0);
        vert.m_Tangent = glm::vec3(0);
        vert.m_Bitangent = glm::vec3(0);
        vert.m_BoneWeights = glm::vec4(0);
        vert.m_BoneIndices = glm::vec4(0);

        vector<pair<int, Weight> > weightArray;
        // Sum the position of the weights
        for ( int j = 0; j < vert.m_WeightCount; ++j ) {
            weightArray.push_back(pair<int, Weight>(vert.m_StartWeight + j, mesh.m_Weights[vert.m_StartWeight + j]));
        }

        std::sort(weightArray.begin(), weightArray.end(), sortWeights);

        // Sum the position of the weights with the highest weight
        for ( int j = 0; j < min((int) weightArray.size(), 4) ; ++j )
        {
            assert( j < 4 );

            const Weight& weight = weightArray[j].second;
            Joint& joint = m_Joints[weight.m_JointID];

            // Convert the weight position from Joint local space to object space
            glm::vec3 rotPos = joint.m_Orient * weight.m_Pos;

            vert.m_Pos += ( joint.m_Pos + rotPos ) * weight.m_Bias;
            vert.m_BoneIndices[j] = (float) weight.m_JointID;
            vert.m_BoneWeights[j] = weight.m_Bias;
        }

        mesh.m_PositionBuffer.push_back(vert.m_Pos);
        mesh.m_Tex2DBuffer.push_back(vert.m_Tex0);
        mesh.m_BoneIndex.push_back(vert.m_BoneIndices);
        mesh.m_BoneWeights.push_back(vert.m_BoneWeights);
    }

    return true;
}

void MD5Model::BuildBindPose( const JointList& joints ) {
    m_BindPose.clear();
    m_InverseBindPose.clear();

    JointList::const_iterator iter = joints.begin();
    while ( iter != joints.end() )
    {
        const Joint& joint = (*iter);
        glm::mat4x4 boneTranslation = glm::translate( joint.m_Pos );
        glm::mat4x4 boneRotation = glm::toMat4( joint.m_Orient );

        glm::mat4x4 boneMatrix = boneTranslation * boneRotation;

        glm::mat4x4 inverseBoneMatrix = glm::inverse( boneMatrix );

        m_BindPose.push_back( boneMatrix );
        m_InverseBindPose.push_back( inverseBoneMatrix );

        ++iter;
    }
}

void MD5Model::update(meow_time_t time) {
    if ( m_bHasAnimation )
    {
        m_Animation.Update(time);

        const MatrixList& animatedSkeleton = m_Animation.GetSkeletonMatrixList();
        // Multiply the animated skeleton joints by the inverse of the bind pose.
        for ( int i = 0; i < m_iNumJoints; ++i )
        {
            m_AnimatedBones[i] = animatedSkeleton[i] * m_InverseBindPose[i];
        }
    }
    else
    {
        // No animation.. Just use identity matrix for each bone.
        m_AnimatedBones.assign(m_iNumJoints, glm::mat4x4(1.0) );
    }

//    for ( unsigned int i = 0; i < m_Meshes.size(); ++i )
//    {
//        // NOTE: This only needs to be done for CPU skinning, but if I want to render the
//        // animated normals, I have to update the mesh on the CPU.
//        PrepareMesh( m_Meshes[i], m_AnimatedBones );
//    }
}

void MD5Model::draw(meow_time_t time, ShaderManager *shaderManager, const glm::mat4 &model,
                      const glm::mat4 &view, const glm::mat4 &projectionView, const glm::vec3 &eyePos,
                      const glm::vec3 &lookAt)
{
    // Drop the very first time
    if (lastTime == 0 || lastTime > time) {
        lastTime = time;
    }

    for ( unsigned int i = 0; i < m_Meshes.size(); ++i )
    {
        update(time - lastTime);
        renderMesh(m_Meshes[i], time, shaderManager, model, view, projectionView, eyePos, lookAt);
    }
}

void MD5Model::initializeBuffers(Mesh & mesh) {
    glGenBuffers(1, &mesh.positionBuffer);
    glGenBuffers(1, &mesh.normalBuffer);
    glGenBuffers(1, &mesh.textcoordBuffer);
    glGenBuffers(1, &mesh.tangentBuffer);
    glGenBuffers(1, &mesh.bitangentBuffer);
    glGenBuffers(1, &mesh.boneWeights);
    glGenBuffers(1, &mesh.boneIndex);
    glGenBuffers(1, &mesh.indexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.positionBuffer);
    checkGlError("glBindBuffer[positionBuffer]");
    glBufferData( GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.m_PositionBuffer.size(), &(mesh.m_PositionBuffer[0]), GL_STATIC_DRAW );
    checkGlError("glBufferData[positionBuffer]");

    glBindBuffer( GL_ARRAY_BUFFER, mesh.normalBuffer);
    checkGlError("glBindBuffer[positionBuffer]");
    glBufferData( GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.m_NormalBuffer.size(), &(mesh.m_NormalBuffer[0]), GL_STATIC_DRAW );
    checkGlError("glBufferData[positionBuffer]");

    glBindBuffer( GL_ARRAY_BUFFER, mesh.tangentBuffer);
    checkGlError("glBindBuffer[tangentBuffer]");
    glBufferData( GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.m_TangentBuffer.size(), &(mesh.m_TangentBuffer[0]), GL_STATIC_DRAW );
    checkGlError("glBufferData[bitangentBuffer]");

    glBindBuffer( GL_ARRAY_BUFFER, mesh.bitangentBuffer );
    checkGlError("glBindBuffer[bitangentBuffer]");
    glBufferData( GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.m_BitangentBuffer.size(), &(mesh.m_BitangentBuffer[0]), GL_STATIC_DRAW );
    checkGlError("glBufferData[bitangentBuffer]");

    glBindBuffer( GL_ARRAY_BUFFER, mesh.textcoordBuffer );
    checkGlError("glBindBuffer[textcoordBuffer]");
    glBufferData( GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh.m_Tex2DBuffer.size(), &(mesh.m_Tex2DBuffer[0]), GL_STATIC_DRAW );
    checkGlError("glBufferData[textcoordBuffer]");

    glBindBuffer( GL_ARRAY_BUFFER, mesh.boneWeights );
    checkGlError("glBindBuffer[boneWeights]");
    glBufferData( GL_ARRAY_BUFFER, sizeof(glm::vec4) * mesh.m_BoneWeights.size(), &(mesh.m_BoneWeights[0]), GL_STATIC_DRAW );
    checkGlError("glBufferData[boneWeights]");

    glBindBuffer( GL_ARRAY_BUFFER, mesh.boneIndex );
    checkGlError("glBindBuffer[boneIndex]");
    glBufferData( GL_ARRAY_BUFFER, sizeof(glm::vec4) * mesh.m_BoneIndex.size(), &(mesh.m_BoneIndex[0]), GL_STATIC_DRAW );
    checkGlError("glBufferData[boneIndex]");

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh.indexBuffer );
    checkGlError("glBindBuffer[indexBuffer]");
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh.m_IndexBuffer.size(), &(mesh.m_IndexBuffer[0]), GL_STATIC_DRAW );
    checkGlError("glBufferData[indexBuffer]");
}

void MD5Model::releaseBuffers(Mesh & mesh) {
    glDeleteBuffers(1, &mesh.positionBuffer);
    glDeleteBuffers(1, &mesh.normalBuffer);
    glDeleteBuffers(1, &mesh.textcoordBuffer);
    glDeleteBuffers(1, &mesh.tangentBuffer);
    glDeleteBuffers(1, &mesh.bitangentBuffer);
    glDeleteBuffers(1, &mesh.boneWeights);
    glDeleteBuffers(1, &mesh.boneIndex);
    glDeleteBuffers(1, &mesh.indexBuffer);

    mesh.m_Weights.clear();
    mesh.m_BitangentBuffer.clear();
    mesh.m_BoneIndex.clear();
    mesh.m_BoneWeights.clear();
    mesh.m_IndexBuffer.clear();
    mesh.m_NormalBuffer.clear();
    mesh.m_Tex2DBuffer.clear();
    mesh.m_TangentBuffer.clear();
    mesh.m_Tris.clear();
    mesh.m_PositionBuffer.clear();
}

void MD5Model::renderMesh(const Mesh &mesh, meow_time_t time, ShaderManager *shaderManager, const glm::mat4 &model,
                          const glm::mat4 &view, const glm::mat4 &projectionView, const glm::vec3 &eyePos,
                          const glm::vec3 &lookAt)
{
    if (shaderManager->hasUniform(shaderID,SHADER_UNIFORM_MODEL_MATRIX)) {
        // Bind the modelview & projection
        glUniformMatrix4fv(shaderManager->getUniform(shaderID, SHADER_UNIFORM_MODEL_MATRIX), 1, GL_FALSE,
                           glm::value_ptr(model));
        checkGlError(SHADER_UNIFORM_MODEL_MATRIX);
    } else {
        ALOGE("ERROR no model matrix uniform!");
    }

    if (shaderManager->hasUniform(shaderID,SHADER_UNIFORM_VIEW_MATRIX)) {
        // Bind the modelview & projection
        glUniformMatrix4fv(shaderManager->getUniform(shaderID, SHADER_UNIFORM_VIEW_MATRIX), 1, GL_FALSE,
                           glm::value_ptr(view));
        checkGlError(SHADER_UNIFORM_VIEW_MATRIX);
    } else {
        ALOGE("ERROR no view matrix uniform!");
    }

    if ( shaderManager->hasUniform(shaderID,SHADER_UNIFORM_PROJECTION_MATRIX)) {
        glUniformMatrix4fv(shaderManager->getUniform(shaderID, SHADER_UNIFORM_PROJECTION_MATRIX), 1, GL_FALSE,
                           glm::value_ptr(projectionView));
        checkGlError(SHADER_UNIFORM_PROJECTION_MATRIX);
    } else {
        ALOGE("ERROR no perspective matrix uniform!");
    }

    if (shaderManager->hasUniform(shaderID, SHADER_UNIFORM_EYE_VEC)) {
        GLuint shaderEyeVec = shaderManager->getUniform(shaderID, SHADER_UNIFORM_EYE_VEC);
        glUniform3fv(shaderEyeVec, 1, glm::value_ptr(eyePos));
        checkGlError("[u_eyeVec]glUniform3fv");
    }

//    if (shaderManager->hasUniform(shaderID, SHADER_UNIFORM_TIME)) {
//        GLfloat timeToShader = time;
//        GLuint shaderUniformTime = shaderManager->getUniform(shaderID, SHADER_UNIFORM_TIME);
//        glUniform1f(shaderUniformTime, timeToShader);
//        checkGlError("[u_time][md5]glUniform1i");
//    }
    glUniformMatrix4fv(shaderManager->getUniform(shaderID, SHADER_UNIFORM_BONE_MATRIX), m_AnimatedBones.size(), GL_FALSE, &(m_AnimatedBones[0][0][0]));

    GLuint weightsIndex = shaderManager->getAttrib(shaderID, SHADER_ATTRIBUTE_WEIGHT);
    glEnableVertexAttribArray(weightsIndex);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.boneWeights);
    glVertexAttribPointer(weightsIndex, 4 , GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    checkGlError("set weights!");

    GLuint positionIndex = shaderManager->getAttrib(shaderID, SHADER_ATTRIBUTE_POSITION);
    glEnableVertexAttribArray(positionIndex);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.positionBuffer);
    glVertexAttribPointer(positionIndex, 3 , GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    checkGlError("send position index");

    GLuint matrixIndexIndex = shaderManager->getAttrib(shaderID, SHADER_ATTRIBUTE_MATRIX_INDEX);
    glEnableVertexAttribArray(matrixIndexIndex);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.boneIndex);
    glVertexAttribPointer(matrixIndexIndex, 4 , GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    checkGlError("send matrix index");

    GLuint uvIndex = shaderManager->getAttrib(shaderID, SHADER_ATTRIBUTE_TEX_COORD);
    glEnableVertexAttribArray(uvIndex);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.textcoordBuffer);
    glVertexAttribPointer(uvIndex, 2, GL_FLOAT, 0, 0, BUFFER_OFFSET(0) );
    checkGlError("send uv coords");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBuffer);
    checkGlError("glBindBuffer[GL_ELEMENT_ARRAY_BUFFER]");
    glDrawElements(GL_TRIANGLES, mesh.m_IndexBuffer.size(), GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
    checkGlError("glDrawElements[drawElements]");
//    glBindTexture( GL_TEXTURE_2D, mesh.m_TexID );
//    glVertexPointer( 3, GL_FLOAT, 0, &(mesh.m_PositionBuffer[0]) );
//    glNormalPointer( GL_FLOAT, 0, &(mesh.m_NormalBuffer[0]) );
//    glTexCoordPointer( 2, GL_FLOAT, 0, &(mesh.m_Tex2DBuffer[0]) );
//
//    glDrawElements( GL_TRIANGLES, mesh.m_IndexBuffer.size(), GL_UNSIGNED_INT, &(mesh.m_IndexBuffer[0]) );
//
//    glDisableClientState( GL_NORMAL_ARRAY );
//    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
//    glDisableClientState( GL_VERTEX_ARRAY );
//
//    glBindTexture( GL_TEXTURE_2D, 0 );
}



