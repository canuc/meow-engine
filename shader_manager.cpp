#include "shader_manager.h"
#include "meow.h"
#include <platform_fopen.h>
#include "util/md5.h"

#define SHADER_NOT_FOUND -1
#define SHADER_COMPILATION_ERROR -2

ShaderManager::~ShaderManager() {
    glUseProgram(0);

    vector<ShaderBundle *>::iterator iter = shadersmanaged.begin();
    while (iter != shadersmanaged.end()) {
        ALOGE("DELETING SHADER: %u, is shader: %s",(*iter)->shader,glIsShader(shaderVec[(*iter)->shader])? "true" : "false");
        if (glIsShader((*iter)->shader)) {
            glDeleteShader((*iter)->shader);
        }
        iter++;
    }
    shadersmanaged.clear();
}

char * ShaderManager::readfile(const string & filename)
{
    FILE * shaderFile = meow_fopen(filename.c_str(),"r");
    fseek(shaderFile, 0L, SEEK_END);
    int fileSize = ftell(shaderFile);
    fseek(shaderFile, 0L, SEEK_SET);
    char * shaderbuffer = new char[fileSize+1];

    fread(shaderbuffer,1,fileSize,shaderFile);
    shaderbuffer[fileSize] = 0x00;
    fclose(shaderFile);
    return shaderbuffer;
}

/*!
 * \brief ShaderManager::loadShader
 * \param shaderType must be one of: GL_VERTEX_SHADER  or GL_FRAGMENT_SHADER
 * \param file the filename to read, on android should be relative to assets dir
 * \return the integer defining the compiled shader
 */
GLuint ShaderManager::loadShader(GLenum shaderType,const string & file) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        char * shaderSrc = readfile(file);
        const char * shaderConstRef = shaderSrc;
        // The shader source must be a handle to the shader
        glShaderSource(shader, 1, &shaderConstRef , NULL);
        delete [] shaderSrc;

        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    ALOGE("Could not compile shader %d:\n%s\n",
                         shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    } else {
        checkGlError("glCreateShader");
        ALOGE("Shader error loading while creating shader: %u",shader);
    }
    return shader;
}

GLint ShaderManager::getAttrib(const int program, const string & attribname)  {
    map<string,GLint>::iterator it = shadersmanaged[program]->attribs.find(attribname);

    // printMap(shadersmanaged[program].attribs);
    if (it != shadersmanaged[program]->attribs.end()) {
        return it->second;
    } else {
        return 0;
    }
}

bool ShaderManager::hasAttrib(const int program,const string & attribname) {
    return shadersmanaged[program]->attribs.find(attribname) != shadersmanaged[program]->attribs.end();
}

bool ShaderManager::hasUniform(const int program,const string & uniformname) {
    return shadersmanaged[program]->uniforms.find(uniformname) != shadersmanaged[program]->uniforms.end();
}

void ShaderManager::printMap(const map<string,GLint> & shaderMap) {
    for(map<string,GLint>::const_iterator it = shaderMap.begin(); it != shaderMap.end(); ++it) {
        ALOGI("KEY: %s",it->first.c_str());
    }

    return;
}

GLint ShaderManager::getUniform(const int program, const string & uniformname )  {
    map<string,GLint>::iterator it = shadersmanaged[program]->uniforms.find(uniformname);

    if (it != shadersmanaged[program]->uniforms.end()) {
        return it->second;
    } else {
        return 0;
    }
}

GLuint ShaderManager::getShaderProgram(const int program) const {
    return shaderVec[shadersmanaged[program]->shader];
}

void ShaderManager::deleteShader( const int program ) {
//    glDeleteShader(shadersmanaged[program].shader);
//    shadersmanaged[]
}

int ShaderManager::compileShader(const string & vertex, const string & fragment) {
    ALOGI("Loading shader with vertex: %s fragment: %s\n",vertex.c_str(),fragment.c_str());

    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertex);
    if (!vertexShader) {
        ALOGE("Vertex shader error shader %s\n",vertex.c_str());
        return SHADER_COMPILATION_ERROR;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, fragment);
    if (!pixelShader) {
        ALOGE("Fragment shader error: %s\n",fragment.c_str());
        return SHADER_COMPILATION_ERROR;
    }

    GLuint program = glCreateProgram();
    int shaderindex = -1;
    if (program) {
        glAttachShader(program, vertexShader);
        glAttachShader(program, pixelShader);

        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    ALOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
        } else {
            string name = shaderName(vertex,fragment);
            ALOGI("Shader name: %s",name.c_str());
            // Okay we've linked the shader, now give go through and find all the
            // uniforms as well as all the attribs
            shaderindex = shaderVec.size();
            shaderVec.push_back(program);

            shaderMap[name] = shaderindex;
        }
    } else {
        ALOGE("Could not create program: %s fragment: %s",vertex.c_str(),fragment.c_str());
    }

    return shaderindex;
}

string ShaderManager::shaderName( const string & vertex, const string & fragment) {
    string vertexFilename = getFilename(vertex);
    string fragmentFilename = getFilename(fragment);

    return md5(vertexFilename + ":" + fragmentFilename);
}

int ShaderManager::getShader( const string & vertex, const string & fragment ) {
    string name = shaderName(vertex, fragment);
    if (shaderMap.find(name) != shaderMap.end()) {
        return shaderMap[name];
    } else {
        return SHADER_NOT_FOUND;
    }
}

void ShaderManager::loadUniforms(ShaderBundle * bundle,vector<string> uniforms) {
    GLuint shader = shaderVec[bundle->shader];

    for( size_t i = 0; i < uniforms.size(); i++ ) {
        GLint uniformlocation = glGetUniformLocation(shader, uniforms[i].c_str());
        if (uniformlocation >= 0 ) {
            bundle->uniforms[uniforms[i]] = uniformlocation;
#ifdef DEBUG_SHADERS
            ALOGI("uniform[%s]: %d\n", uniforms[i].c_str(), uniformlocation);
#endif
        } else {
#ifdef DEBUG_SHADERS
            ALOGI("invalid uniform[%s]: %d\n", uniforms[i].c_str(), uniformlocation);
#endif
        }
    }
}

void ShaderManager::loadAttributes(ShaderBundle * bundle, vector<string> attributes) {
    GLuint shader = shaderVec[bundle->shader];
#ifdef DEBUG_SHADERS
    ALOGI("loading attributes: %u\n", shader);
#endif
    for( size_t i = 0; i < attributes.size(); i++ ) {
        GLint attriblocation = glGetAttribLocation(shader, attributes[i].c_str());
        if (attriblocation >= 0 ) {
            bundle->attribs[attributes[i]] = attriblocation;
#ifdef DEBUG_SHADERS
            ALOGI("attribs[%s]: %d\n", attributes[i].c_str() ,attriblocation);
#endif
        } else {
#ifdef DEBUG_SHADERS
            ALOGI("invalid attribs[%s]: %d\n", attributes[i].c_str(), attriblocation);
#endif
        }
#ifdef DEBUG_SHADERS
        ALOGI("attribs[%s]: %d\n",attributes[i].c_str(),attriblocation);
#endif
    }
}

int ShaderManager::createShader( const string & vertex, const string & fragment, vector<string> attributes, vector<string> uniforms) {
    int shaderIndex = getShader(vertex, fragment);
    if ( shaderIndex == SHADER_NOT_FOUND) {
        shaderIndex = compileShader(vertex, fragment);
    }

    if ( shaderIndex == SHADER_COMPILATION_ERROR) {
        ALOGE("Shader compilation error!");
        return SHADER_NOT_FOUND;
    }

    ALOGI("Shaderindex: %d",shaderIndex);

    ShaderBundle * bundle = new ShaderBundle();
    bundle->shader = shaderIndex;

    loadAttributes(bundle,attributes);
    loadUniforms(bundle,uniforms);
    int shaderBundleIndex = shadersmanaged.size();
    shadersmanaged.push_back(bundle);

    return shaderBundleIndex;
}
