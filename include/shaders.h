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

#ifndef NYAN_ANDROID_SHADERS_H
#define NYAN_ANDROID_SHADERS_H

////////////////////////////////////////////////////
// Attributes
////////////////////////////////////////////////////

#define SHADER_ATTRIBUTE_POSITION "a_Position"
#define SHADER_ATTRIBUTE_COLOR "a_Color"
#define SHADER_ATTRIBUTE_TEX_COORD "a_texCoord"
#define SHADER_ATTRIBUTE_NORMAL "a_normal"
#define SHADER_ATTRIBUTE_BITANGENT "a_bitangent"
#define SHADER_ATTRIBUTE_TANGENT "a_tangent"

////////////////////////////////////////////////////
// GPU Animated Model Attributes
////////////////////////////////////////////////////

#define SHADER_ATTRIBUTE_MATRIX_INDEX "a_matrixIndex"
#define SHADER_ATTRIBUTE_WEIGHT "a_weight"

////////////////////////////////////////////////////
// Uniforms
////////////////////////////////////////////////////

#define SHADER_UNIFORM_MODEL_MATRIX "u_MMatrix"
#define SHADER_UNIFORM_PROJECTION_MATRIX "u_PMatrix"
#define SHADER_UNIFORM_VIEW_MATRIX "u_VMatrix"
#define SHADER_UNIFORM_TEXTURE_ID "u_texture"
#define SHADER_UNIFORM_BUMP_ID "u_bump"
#define SHADER_UNIFORM_EYE_VEC "u_eyeVec"
#define SHADER_UNIFORM_POSITION "u_position"
#define SHADER_UNIFORM_COLOR "u_color"
#define SHADER_UNIFORM_TIME "u_time"
#define SHADER_UNIFORM_BLUR_HORIZONTAL "u_horizontal"

////////////////////////////////////////////////////
// GPU Animated Model Uniforms
////////////////////////////////////////////////////

#define SHADER_UNIFORM_BONE_MATRIX "u_boneMatrix"


#endif //NYAN_ANDROID_SHADERS_H
