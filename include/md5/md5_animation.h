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

#ifndef NYAN_ANDROID_MD5_ANIMATION_H
#define NYAN_ANDROID_MD5_ANIMATION_H

#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <meow_gl.h>
#include "util/quat.h"
#include <sstream>
#include "timer/timer.h"

class MD5Animation
{
public:
    MD5Animation();
    virtual ~MD5Animation();

    // Load an animation from the animation file
    bool LoadAnimation( const std::string& filename );
    // update this animation's joint set.
    void Update( meow_time_t drawTime );
//    // Draw the animated skeleton
//    void render();

    // The JointInfo stores the information necessary to build the
    // skeletons for each frame
    struct JointInfo
    {
        std::string m_Name;
        int         m_ParentID;
        int         m_Flags;
        int         m_StartIndex;
    };
    typedef std::vector<JointInfo> JointInfoList;

    struct Bound
    {
        glm::vec3   m_Min;
        glm::vec3   m_Max;
    };
    typedef std::vector<Bound> BoundList;

    struct BaseFrame
    {
        glm::vec3   m_Pos;
        glm::quat   m_Orient;
    };
    typedef std::vector<BaseFrame> BaseFrameList;

    struct FrameData
    {
        int m_iFrameID;
        std::vector<float> m_FrameData;
    };
    typedef std::vector<FrameData> FrameDataList;

    // A Skeleton joint is a joint of the skeleton per frame
    struct SkeletonJoint
    {


        SkeletonJoint()
                : m_Parent(-1)
                , m_Pos(0)
        {}

        SkeletonJoint( const BaseFrame& copy )
                : m_Pos( copy.m_Pos )
                , m_Orient( copy.m_Orient )
        {}

        int         m_Parent;
        glm::vec3   m_Pos;
        glm::quat   m_Orient;
    };
    typedef std::vector<SkeletonJoint> SkeletonJointList;
    typedef std::vector<glm::mat4x4> SkeletonMatrixList;

    // A frame skeleton stores the joints of the skeleton for a single frame.
    struct FrameSkeleton
    {
        SkeletonMatrixList  m_BoneMatrices;
        SkeletonJointList   m_Joints;
    };

    typedef std::vector<FrameSkeleton> FrameSkeletonList;

    const FrameSkeleton& GetSkeleton() const
    {
        return m_AnimatedSkeleton;
    }

    const SkeletonMatrixList& GetSkeletonMatrixList() const
    {
        return m_AnimatedSkeleton.m_BoneMatrices;
    }

    int GetNumJoints() const
    {
        return m_iNumJoints;
    }

    const JointInfo& GetJointInfo( unsigned int index ) const
    {
        assert( index < m_JointInfos.size() );
        return m_JointInfos[index];
    }

protected:

    JointInfoList       m_JointInfos;
    BoundList           m_Bounds;
    BaseFrameList       m_BaseFrames;
    FrameDataList       m_Frames;
    FrameSkeletonList   m_Skeletons;    // All the skeletons for all the frames
    FrameSkeleton       m_AnimatedSkeleton;

    // Build the frame skeleton for a particular frame
    void BuildFrameSkeleton( FrameSkeletonList& skeletons, const JointInfoList& jointInfo, const BaseFrameList& baseFrames, const FrameData& frameData );
    void InterpolateSkeletons( FrameSkeleton& finalSkeleton, const FrameSkeleton& skeleton0, const FrameSkeleton& skeleton1, float fInterpolate );


private:
    int m_iMD5Version;
    int m_iNumFrames;
    int m_iNumJoints;
    int m_iFramRate;
    int m_iNumAnimatedComponents;

    float m_fAnimDuration;
    float m_fFrameDuration;
    float m_fAnimTime;
};
#endif //NYAN_ANDROID_MD5_ANIMATION_H
