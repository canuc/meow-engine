#include "meow_util.h"
/*!
 * \internal
 * \brief readline will read until a UNIX new-line is reached
 * \param io the sdl file pointer
 * \param readbuffer a buffer to read into
 * \return the number of bytes read, EOF_SENTINEL if no bytes available
 */
 int readline(FILE * io, char * readbuffer) {
    int currentbufferpos = 0;
    int characterbuffer = 0;
    do {

        characterbuffer = fgetc(io);

        if ( characterbuffer !=  '\n' && characterbuffer != EOF ) {
            if (readbuffer != NULL) {
                readbuffer[currentbufferpos] = (char) characterbuffer;
            }
        } else {
            break;
        }

        // add the item onto the line
        currentbufferpos += 1;

    } while (characterbuffer != 0 );

    if ( characterbuffer == EOF ) {
        return EOF_SENTINEL;
    }

    return currentbufferpos;
}

void unquote( std::string& str )
{
    size_t n;
    while ( ( n = str.find('\"') ) != std::string::npos ) str.erase(n,1);
}

#ifdef USE_ANDROID


char * readall(AAssetManager * assetManager,const string & filename) {
    AAsset * rwOps = AAssetManager_open(assetManager, (const char *) filename.c_str(), AASSET_MODE_UNKNOWN);
    off64_t filesize = AAsset_getLength64(rwOps);
    char * filebytes = new char[filesize+1];
    memset(filebytes, 0x00, filesize+1);
    AAsset_read(rwOps, filebytes, filesize);
    AAsset_close(rwOps);
    return filebytes;
}

#else
int readline(SDL_RWops * io, char * readbuffer) {
    int currentbufferpos = 0;
    int read = 0;

    char characterbuffer = 0;
    do {

        read = io->read(io,&characterbuffer,1,1);

        if ( characterbuffer != '\n' ) {
            readbuffer[currentbufferpos] = characterbuffer;
        } else {
            break;
        }

        // add the item onto the line
        currentbufferpos += read;

    } while (read != 0 );

    if ( read == 0 ) {
        return EOF_SENTINEL;
    }

    return currentbufferpos;
}

#endif
static string relativeFilepathBase;

string getFilepath(const string & filepath) {
    size_t relativeFilePosition = filepath.find_last_of('/');
    string relativeFilePath = "";

    if (relativeFilePosition != string::npos) {
        relativeFilePath = filepath.substr(0, relativeFilePosition+1);
    }

    return relativeFilePath;
}

string getFilename(const string & filepath) {
    size_t relativeFilePosition = filepath.find_last_of('/');
    string relativeFilePath = "";

    if (relativeFilePosition != string::npos) {
        relativeFilePath = filepath.substr(relativeFilePosition+1,filepath.size());
    }

    return relativeFilePath;
}

void setRelativeBase(string relativeBase) {
    relativeFilepathBase = relativeBase;
}

string getRelativeLocation(const string & filedir,const string & filename) {
    string relativeFilename = "";
    if (filename.size() > 0 && filename[0] == '/') {
        relativeFilename = filename;
        relativeFilename = relativeFilepathBase + relativeFilename;
    } else {
        relativeFilename = filedir + filename;
    }

    if (relativeFilename.size() > 0 && relativeFilename[0] == '/') {
        relativeFilename = relativeFilename.substr(1);
    }

    return relativeFilename;
}

double unitToMeters(double units) {
    return units * M_TO_UNITS;
}

