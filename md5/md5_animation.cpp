//
// Created by julian on 5/7/16.
//

#include "md5/md5_animation.h"
#include "meow.h"

MD5Animation::MD5Animation()
        : m_iMD5Version( 0 )
        , m_iNumFrames( 0 )
        , m_iNumJoints( 0 )
        , m_iFramRate( 0 )
        , m_iNumAnimatedComponents( 0 )
        , m_fAnimDuration( 0 )
        , m_fFrameDuration( 0 )
        , m_fAnimTime( 0 )
{
}


MD5Animation::~MD5Animation()
{}

bool MD5Animation::LoadAnimation( const std::string& filename ) {
    FILE *fp = meow_fopen(filename.c_str(),"r");
    char  buff[LINE_BUFFER_SIZE] = {0};
    int frame_index;

    if (!fp) {
        ALOGE("MD5Animation::LoadAnimation: Failed to find file: %s", filename.c_str());
        return false;
    }

    while (!feof (fp))
    {
        /* Read whole line */
        fgets (buff, sizeof (buff), fp);
        if (sscanf (buff, "MD5Version %d", &m_iMD5Version) == 1) {
            if (m_iMD5Version != 10) {
                ALOGI("MD5 Version is incorrect. Required Version is 10. currently: %d", m_iMD5Version);
                goto clean_up_file;
            }
        } else if (sscanf (buff, "numFrames %d", &m_iNumFrames) == 1) {
            /* Allocate memory for skeleton frames and bounding boxes */
            if (m_iNumFrames > 0)
            {
                m_Frames.reserve(m_iNumFrames);
            } else {
                ALOGI("MD5 number of frames is incorrect. must be > 0 %d", m_iNumFrames);
                goto clean_up_file;
            }
        } else if (sscanf (buff, "numJoints %d", &m_iNumJoints) == 1) {
            if (m_iNumJoints > 0) {
                m_JointInfos.reserve(m_iNumJoints);
            } else {
                ALOGI("MD5 number of joints incorrect. must be > 0 %d", m_iNumJoints);
                goto clean_up_file;
            }
        } else if (sscanf (buff, "frameRate %d", &m_iFramRate) == 1) {
            if (m_iFramRate <= 0) {
                ALOGI("Frame rate is incorrect found %d, must be > 0", m_iFramRate);
            }
        } else if (sscanf (buff, "numAnimatedComponents %d", &m_iNumAnimatedComponents) == 1) {
            if (m_iNumAnimatedComponents <= 0) {
                ALOGI("Number of animated components: %d, must be > 0", m_iNumAnimatedComponents);
            }
        } else if (strncmp (buff, "hierarchy {", 11) == 0) {

            for ( int i = 0; i < m_iNumJoints; ++i )
            {
                JointInfo joint;
                fgets (buff, sizeof (buff), fp);
                char * currentBufferOffset = buff;

                /* trim the whitespace */
                while(isspace((unsigned char)*currentBufferOffset)) currentBufferOffset++;

                char nameBuffer[1024] = {0};
                sscanf (currentBufferOffset, "\"%[^\"]\" %d %d %d", nameBuffer, &joint.m_ParentID,
                        &joint.m_Flags, &joint.m_StartIndex);
                joint.m_Name = nameBuffer;
                m_JointInfos.push_back(joint);
            }
        } else if (strncmp (buff, "bounds {", 8) == 0) {
            for (int i = 0; i < m_iNumFrames; ++i) {
                Bound newBound;
                /* Read whole line */
                fgets (buff, sizeof (buff), fp);
                char * currentBufferOffset = buff;

                /* trim the whitespace */
                while(isspace((unsigned char)*currentBufferOffset)) currentBufferOffset++;

                /* Read bounding box */
                sscanf (currentBufferOffset, "( %f %f %f ) ( %f %f %f )",
                        &newBound.m_Min.x, &newBound.m_Min.y, &newBound.m_Min.z,
                        &newBound.m_Max.x, &newBound.m_Max.y, &newBound.m_Max.z);

                m_Bounds.push_back(newBound);
            }
        } else if (strncmp (buff, "baseframe {", 10) == 0) {
            for (int i = 0; i < m_iNumJoints; ++i)
            {
                /* Read whole line */
                fgets (buff, sizeof (buff), fp);
                char * currentBufferOffset = buff;

                /* trim the whitespace */
                while(isspace((unsigned char)*currentBufferOffset)) currentBufferOffset++;

                BaseFrame baseFrame;
                /* Read base frame joint */
                if (sscanf (currentBufferOffset, " ( %f %f %f ) ( %f %f %f )",
                            &baseFrame.m_Pos.x, &baseFrame.m_Pos.y, &baseFrame.m_Pos.z,
                            &baseFrame.m_Orient.x, &baseFrame.m_Orient.y, &baseFrame.m_Orient.z) == 6)
                {
                    /* Compute the w component */
                    ComputeQuatW(baseFrame.m_Orient);
                    m_BaseFrames.push_back(baseFrame);
                }
            }
        } else if (sscanf (buff, "frame %d", &frame_index) == 1) {
            float frameWeight;
            double frameDoubleWeight;

            FrameData frame;
            frame.m_iFrameID = frame_index;
            frame.m_FrameData.reserve(m_iNumAnimatedComponents);

            for (int i = 0; i < m_iNumAnimatedComponents; ++i) {
                fscanf(fp, "%lf", &frameDoubleWeight);
                frameWeight = float(frameDoubleWeight);
                frame.m_FrameData.push_back(frameWeight);
            }

            /* Build frame skeleton from the collected data */
            BuildFrameSkeleton( m_Skeletons, m_JointInfos, m_BaseFrames, frame );

            m_Frames.push_back(frame);
        }
    }

    m_AnimatedSkeleton.m_Joints.assign(m_iNumJoints, SkeletonJoint() );     // DEPRICATED
    m_AnimatedSkeleton.m_BoneMatrices.assign( m_iNumJoints, glm::mat4x4(1.0) );

    m_fFrameDuration = 1000.0f / (float) m_iFramRate;
    m_fAnimDuration = ( m_fFrameDuration * (float)m_iNumFrames );
    m_fAnimTime = 0.0f;

    clean_up_file:
        fclose(fp);

    return true;
}

void MD5Animation::BuildFrameSkeleton( FrameSkeletonList& skeletons, const JointInfoList& jointInfos, const BaseFrameList& baseFrames, const FrameData& frameData ) {
    FrameSkeleton skeleton;

    for ( unsigned int i = 0; i < jointInfos.size(); ++i )
    {
        unsigned int j = 0;

        const JointInfo& jointInfo = jointInfos[i];
        // Start with the base frame position and orientation.
        SkeletonJoint animatedJoint = baseFrames[i];

        animatedJoint.m_Parent = jointInfo.m_ParentID;

        if ( jointInfo.m_Flags & 1 ) // Pos.x
        {
            animatedJoint.m_Pos.x = frameData.m_FrameData[ jointInfo.m_StartIndex + j++ ];
        }
        if ( jointInfo.m_Flags & 2 ) // Pos.y
        {
            animatedJoint.m_Pos.y = frameData.m_FrameData[ jointInfo.m_StartIndex + j++ ];
        }
        if ( jointInfo.m_Flags & 4 ) // Pos.x
        {
            animatedJoint.m_Pos.z  = frameData.m_FrameData[ jointInfo.m_StartIndex + j++ ];
        }
        if ( jointInfo.m_Flags & 8 ) // Orient.x
        {
            animatedJoint.m_Orient.x = frameData.m_FrameData[ jointInfo.m_StartIndex + j++ ];
        }
        if ( jointInfo.m_Flags & 16 ) // Orient.y
        {
            animatedJoint.m_Orient.y = frameData.m_FrameData[ jointInfo.m_StartIndex + j++ ];
        }
        if ( jointInfo.m_Flags & 32 ) // Orient.z
        {
            animatedJoint.m_Orient.z = frameData.m_FrameData[ jointInfo.m_StartIndex + j++ ];
        }

        ComputeQuatW( animatedJoint.m_Orient );

        if ( animatedJoint.m_Parent >= 0 ) // Has a parent joint
        {
            assert(animatedJoint.m_Parent < i);
            SkeletonJoint& parentJoint = skeleton.m_Joints[animatedJoint.m_Parent];
            glm::vec3 rotPos = parentJoint.m_Orient * animatedJoint.m_Pos;

            animatedJoint.m_Pos = parentJoint.m_Pos + rotPos;
            animatedJoint.m_Orient = parentJoint.m_Orient * animatedJoint.m_Orient;

            animatedJoint.m_Orient = glm::normalize( animatedJoint.m_Orient );
        }

        skeleton.m_Joints.push_back(animatedJoint);

        // Build the bone matrix for GPU skinning.
        glm::mat4x4 boneTranslate = glm::translate( animatedJoint.m_Pos );
        glm::mat4x4 boneRotate = glm::toMat4( animatedJoint.m_Orient );
        glm::mat4x4 boneMatrix = boneTranslate * boneRotate;

        skeleton.m_BoneMatrices.push_back(boneMatrix);

    }

    m_Skeletons.push_back(skeleton);
}


void MD5Animation::Update( meow_time_t drawTime )
{
    if ( m_iNumFrames < 1 ) return;

    m_fAnimTime = drawTime;

    while ( m_fAnimTime > m_fAnimDuration ) m_fAnimTime -= m_fAnimDuration;
    while ( m_fAnimTime < 0.0f ) m_fAnimTime += m_fAnimDuration;

    // Figure out which frame we're on
    float fFramNum = (m_fAnimTime/ 1000.0f) * (float)m_iFramRate;
    int iFrame0 = (int)floorf( fFramNum );
    int iFrame1 = (int)ceilf( fFramNum );
    iFrame0 = iFrame0 % m_iNumFrames;
    iFrame1 = iFrame1 % m_iNumFrames;

    float fInterpolate = fmodf( m_fAnimTime, m_fFrameDuration ) / m_fFrameDuration;
    ALOGI("Frame Number: %d, %d, %f, %ld",iFrame0, iFrame1, fInterpolate, drawTime);

    InterpolateSkeletons( m_AnimatedSkeleton, m_Skeletons[iFrame0], m_Skeletons[iFrame1], fInterpolate );
}

void MD5Animation::InterpolateSkeletons( FrameSkeleton& finalSkeleton, const FrameSkeleton& skeleton0, const FrameSkeleton& skeleton1, float fInterpolate )
{
    for ( int i = 0; i < m_iNumJoints; ++i )
    {
        SkeletonJoint& finalJoint = finalSkeleton.m_Joints[i];
        glm::mat4x4& finalMatrix = finalSkeleton.m_BoneMatrices[i];

        const SkeletonJoint& joint0 = skeleton0.m_Joints[i];
        const SkeletonJoint& joint1 = skeleton1.m_Joints[i];

        finalJoint.m_Parent = joint0.m_Parent;

        finalJoint.m_Pos = glm::lerp( joint0.m_Pos, joint1.m_Pos, fInterpolate );
        finalJoint.m_Orient = glm::mix( joint0.m_Orient, joint1.m_Orient, fInterpolate );

        // Build the bone matrix for GPU skinning.
        finalMatrix = glm::translate( finalJoint.m_Pos ) * glm::toMat4( finalJoint.m_Orient );
    }
}
