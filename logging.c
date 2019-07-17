#include "logging.h"

void logger_standard(const char * log, int bufferSize) {
    char * logmessage = (char *) malloc((bufferSize+1) * sizeof(char));
    memcpy(logmessage,log,bufferSize);
    logmessage[bufferSize] = (char) 0;
    DEBUG_LOG("%s",logmessage);
    free(logmessage);
}

void logger_error(const char * log, int bufferSize) {
    char * logmessage = (char *) malloc((bufferSize+1) * sizeof(char));
    memcpy(logmessage,log,bufferSize);
    logmessage[bufferSize] = (char) 0;
    ERROR_LOG("%s",logmessage);
    free(logmessage);
}

void format_error(const char * format, const char * str) {
    #if defined(USE_SDL)
        fprintf(stderr,format,str);
    #elif defined(USE_ANDROID)
        (void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG,format,str);
    #endif // USE_ANDROID
}
