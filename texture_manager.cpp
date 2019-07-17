#include "texture_manager.h"
#include "texture_loading.h"

GLuint TextureManager::loadTexture(const string &filepath) {
    string filename = getFilename(filepath);

    if (textures.find(filename) != textures.end()) {
        ALOGI("Using texture: '%s'\n",filename.c_str());
        return textures[filename];
    } else {
        ALOGI("Loading texture: '%s' named: '%s'\n",filepath.c_str(),filename.c_str());
        GLuint loadedTexture = load(filepath);
        textures[filename] = loadedTexture;
        return loadedTexture;
    }
}

GLuint TextureManager::load(const string & file) {
    return texture_load(gameEngine,file.c_str());
}

TextureManager::~TextureManager() {
    map<string,GLuint>::iterator iter = textures.begin();
    while (iter != textures.end()) {
        glDeleteTextures(1,&iter->second);
        iter++;
    }
    textures.clear();
}