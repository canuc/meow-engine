#ifndef GL_UTIL_H
#define GL_UTIL_H
#define MEOW_MAX(a,b) ((a) > (b) ? (a) : (b))
#define MEOW_MIN(a,b) ((a) < (b) ? (a) : (b))

void checkGlError(const char* funcName);
#endif
