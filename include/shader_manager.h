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

#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

using namespace std;

#include "meow.h"
#include <vector>
#include <map>
#include <string>

#define SHADER_NOT_FOUND -1

class ShaderManager
{
private:
    struct ShaderBundle {
        int shader; //!< Reference to the shaderVec index
        map<string,GLint> attribs;
        map<string,GLint> uniforms;
    };

    vector<ShaderBundle *> shadersmanaged;
    vector<GLuint> shaderVec;      //!< Base Shder vec all indexes
    map<string,int> shaderMap;  //!< Map of shadernames (MD5) to the vec index
    struct engine * android_engine;

protected:
    char * readfile(const string & filename);
    GLuint loadShader(GLenum shaderType, const string & file);
    string shaderName(const string &vertex, const string &fragment);

    void loadUniforms(ShaderBundle * bundle, vector<string> uniforms);
    void loadAttributes(ShaderBundle * bundle, vector<string> attributes);

    /*!
     * \brief createShader create a link a shader with the specific attributes
     * \param vertex the string file location of the vertex shader, should be relative to assets
     * \param fragment the string file location of the fragment shader, should be relative to the assets
     * \param attribs a string list of shader attributes
     * \return the index of the created shader, or -1 upon failure
     */
    int compileShader(const string & vertex, const string & fragment);

public:
    ShaderManager(engine * a_engine):android_engine(a_engine) {};
    ~ShaderManager();

    struct engine * getEngine() { return android_engine; }

    /*!
     * \brief deleteShader deletes the shader specified by index.
     * \param program the id of the program.
     */
    void deleteShader( const int program );

    /*!
     * \brief getShader finds the shader id specified by the string name
     * \param shaderName the name of the shader to retrieve
     */
    int getShader( const string & vertex, const string & fragment );

    int createShader( const string & vertex, const string & fragment, vector<string> attributes, vector<string> uniforms);
    bool hasAttrib(const int program,const string & attribname);
    bool hasUniform(const int program,const string & uniformname);

    GLint getAttrib(const int attrib,const string & attribname );
    GLint getUniform(const int program,const string & uniformname );
    GLuint getShaderProgram( const int program) const;

    void printMap(const map<string,GLint> & map);

};

#endif // SHADERMANAGER_H
