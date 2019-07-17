#include "util/glutil.h"
#include "meow.h"

void checkGlError(const char* funcName) {
    GLint err = glGetError();
    if (err != GL_NO_ERROR) {
        #if USE_GLU
            ALOGE("GL error after %s(): %d: %s\n", funcName, err, gluErrorString(err));
        #else
            ALOGE("GL error after %s(): %d \n", funcName, err);
        #endif
    }
}