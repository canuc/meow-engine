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

#ifndef WAVEFRONTOBJ_H
#define WAVEFRONTOBJ_H
using namespace std;
#include "meow.h"
#include <string>
#include <vector>
#include <map>

#include "shader_manager.h"
#include "drawable.h"
#include "ibounded.h"
#include "imodel.h"

struct ObjGroup;
struct mtl;

class WavefrontObj: public IModel
{


public:
    #ifdef USE_ANDROID
        WavefrontObj(struct engine * app_engine, const string & file);
    #else
        WavefrontObj(const string & file);
    #endif
    ~WavefrontObj();

    virtual void draw(meow_time_t time, ShaderManager *shaderManager, const glm::mat4 &model,
              const glm::mat4 &view, const glm::mat4 &projectionView, const glm::vec3 &eyePos,
              const glm::vec3 &lookAt);

    virtual BoundingRect getBoundingRect() const;
    virtual BoundingSphere getBoundingSphere() const;

protected:
    void loadObj(const string & file);
    void loadMtl(const string & file, map<string,mtl> & mtls);

private:
    GLuint vbo;
    GLuint indexBuffer;
    size_t numberOfVertecies;
    string filename;

    BoundingRect m_bounds;
    BoundingSphere m_boundedSphere;

#ifdef USE_ANDROID
    struct engine * app_engine;
#endif

};

#endif // WAVEFRONTOBJ_H
