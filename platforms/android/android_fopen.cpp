#ifdef USE_ANDROID
#include <stdio.h>
#include "logging.h"
#include "android_fopen.h"
#include <errno.h>
#include "meow_util.h"
#define ANDROID_FILE_URI_PREFIX "file://"
static int android_read(void* cookie, char* buf, int size) {
  return AAsset_read((AAsset*)cookie, buf, size);
}

static int android_write(void* cookie, const char* buf, int size) {
  return EACCES; // can't provide write access to the apk
}

static fpos_t android_seek(void* cookie, fpos_t offset, int whence) {
  return AAsset_seek((AAsset*)cookie, offset, whence);
}

static int android_close(void* cookie) {
  AAsset_close((AAsset*)cookie);
  return 0;
}

// must be established by someone else...
static AAssetManager* android_asset_manager;
void android_fopen_set_asset_manager(AAssetManager* manager) {
  android_asset_manager = manager;
}

FILE* android_fopen(const char* fname, const char* mode) {
  if (strncmp(fname, ANDROID_FILE_URI_PREFIX, strlen(ANDROID_FILE_URI_PREFIX)) == 0) {
      size_t filename_size = strlen(fname)-strlen(ANDROID_FILE_URI_PREFIX) + 1;
      char * fileLocation = new char[filename_size];
      strncpy(fileLocation,fname+strlen(ANDROID_FILE_URI_PREFIX),filename_size);
      FILE * openedFile = fopen(fileLocation,mode);
      delete [] fileLocation;

      return openedFile;
  }
  if(mode[0] == 'w') return NULL;
  AAsset* asset = AAssetManager_open(android_asset_manager, getRelativeLocation("",fname).c_str(), 0);
  if(!asset) return NULL;

  return funopen(asset, android_read, android_write, android_seek, android_close);
}
#endif
