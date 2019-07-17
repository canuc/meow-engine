#include "model_manager.h"

ModelManager::ModelManager(ShaderManager * shaderManager) {
    this->shaderManager = shaderManager;
}

ModelManager::~ModelManager() {
    map<string,WavefrontObj *>::iterator iter = models.begin();
    while (iter != models.end()) {
        delete iter->second;
        iter++;
    }
    models.clear();
}

WavefrontObj * ModelManager::loadModel(const string & filepath) {
    string filename = getFilename(filepath);

    if (models.find(filename) != models.end()) {
        ALOGI("Using model: '%s'\n",filename.c_str());
        return models[filename];
    } else {
        ALOGI("Loading model: '%s' named: '%s'\n",filepath.c_str(),filename.c_str());
        #ifdef USE_ANDROID
        WavefrontObj * wavefront = new WavefrontObj(shaderManager->getEngine(),filepath);
        #else
        WavefrontObj * wavefront = new WavefrontObj(filepath);
        #endif
        models[filename] = wavefront;
        return wavefront;
    }
}

