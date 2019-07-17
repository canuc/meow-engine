#include "font_manager.h"
#include <sstream>
#include <shaders.h>
#include <shader_manager.h>
#include <texture_loading.h>
#include "freetype/ftsnames.h"
#include <algorithm>

#define SHADER_FRAG_TEXT "/shaders/text.frag"
#define SHADER_VERT_TEXT "/shaders/text.vert"

#define LANGUAGE_ID 0
#define NAME_FONT_NAME 1

struct meow_font {
    GLuint font_texture;
    float pt;
    float advance[128];
    float width[128];
    float height[128];
    float tex_x1[128];
    float tex_x2[128];
    float tex_y1[128];
    float tex_y2[128];
    float offset_x[128];
    float offset_y[128];
    string name;

    int initialized;
};

struct meow_text {
    GLuint font_index_buffer;
    GLuint font_vertex_data;
    const meow_font * font;
    int vertecies;
    GLfloat width;
    GLfloat height;
};

struct meow_vertex_buffer {
    GLfloat pos[2];
    GLfloat tex[2];
};

static inline int
nextp2(int x)
{
    int val = 1;
    while(val < x) val <<= 1;
    return val;
}

static void releaseText(meow_text * text) {
    glDeleteBuffers(2,&text->font_index_buffer);
    delete text;
}

static vector<string> getFontUniforms() {
    vector<string> uniforms;
    uniforms.push_back(SHADER_UNIFORM_MODEL_MATRIX);
    uniforms.push_back(SHADER_UNIFORM_PROJECTION_MATRIX);
    uniforms.push_back(SHADER_UNIFORM_TEXTURE_ID);
    uniforms.push_back(SHADER_UNIFORM_COLOR);
    return uniforms;
}

static vector<string> getFontAttribs() {
    vector<string> attribs;
    attribs.push_back(SHADER_ATTRIBUTE_POSITION);
    attribs.push_back(SHADER_ATTRIBUTE_TEX_COORD);
    return attribs;
}

FontManager::FontManager(ShaderManager *shader, int width, int height, int density): texts(30, releaseText), screenWidth(width), screenHeight(height), screenDensity(density) {
    FT_Error errorCode = FT_Init_FreeType(&library);
    shaderManager = shader;
    if ( errorCode ) {
        ALOGE("ERROR with the freetype library %d",errorCode);
        return;
    }
    gameEngine = shaderManager->getEngine();
    textShader = shaderManager->createShader(getRelativeLocation("",SHADER_VERT_TEXT),getRelativeLocation("",SHADER_FRAG_TEXT),getFontAttribs(),getFontUniforms());
}

FontManager::~FontManager() {
    fonts_t::iterator iter = fonts.begin();

    while (iter!= fonts.end()) {
        glDeleteTextures(1,&iter->second->font_texture);
        delete iter->second;
        iter++;
    }

    fonts.clear();

    FT_Done_FreeType(library);
}

int32_t FontManager::loadFont(string filename,int point_size) {
    meow_font * font = loadFontData(filename, point_size, screenDensity);
    int32_t font_idx = -1;

    if (font) {
        fonts[filename] = font;
        font_idx = weak_ref.size();
        weak_ref.push_back(font);
        font->initialized = font_idx;

        if (font->name.size()) {
            font_names[font->name] = font_idx;
        }
    }

    return font_idx;
}

string getStringFromNotNULLString(FT_Byte * buffer,int size) {
    char * nullTerminatedString = new char[size+1];
    memcpy(nullTerminatedString,buffer,size);
    nullTerminatedString[size] = 0;

    string stringName= string(nullTerminatedString);
    delete []nullTerminatedString;
    return stringName;
}

meow_font * FontManager::loadFontData(string filename,int32_t point_size,uint32_t DPI) {
    int MAX_WIDTH_HEIGHT = 0;
    checkGlError("BeforeLoadingFont");
    glGetIntegerv(GL_MAX_TEXTURE_SIZE,&MAX_WIDTH_HEIGHT);

    FT_Face loadingFace;
    int segment_size_x = 0, segment_size_y = 0;
    int num_segments_x = 16;
    int num_segments_y = 8;
    FT_GlyphSlot slot;
    FT_Bitmap bmp;
    int glyph_width, glyph_height;
    int bitmap_offset_x = 0, bitmap_offset_y = 0;
    int font_tex_width = 0;
    int font_tex_height = 0;
    GLubyte* font_texture_data = NULL;
    FT_Error errorCode = FT_New_Face(library, filename.c_str(), 0, &loadingFace);

    if ( errorCode ) {
        ALOGE("ERROR with the freetype face code: %d, %s", errorCode, filename.c_str());
        return NULL;
    }

    int nameCount = FT_Get_Sfnt_Name_Count(loadingFace);
    std::string fontname;

    for (unsigned int i = 0; i < nameCount; i++) {
        FT_SfntName name;
        FT_Get_Sfnt_Name(loadingFace,i,&name);
        string fontName = getStringFromNotNULLString(name.string,name.string_len);
        if ( name.language_id == LANGUAGE_ID && name.name_id == NAME_FONT_NAME) {
            fontname = getStringFromNotNULLString(name.string, name.string_len);
            ALOGE("Got font name[%d][%d]: %s ",name.name_id,name.language_id,fontName.c_str());
        }
    }

    meow_font * font = new meow_font();
    font->initialized = -1;
    errorCode = FT_Set_Char_Size(loadingFace, point_size * 64, point_size * 64, DPI, DPI);
    if(errorCode) {
        ALOGE("Error initializing character parameters: %d\n", errorCode);
        goto CLEANUP_FONT_MANAGER;
    }

    /*First calculate the max width and height of a character in a passed font*/
    for(FT_ULong c = 0; c < 128; c++) {
        errorCode = FT_Load_Char(loadingFace, c, FT_LOAD_RENDER);
        if(errorCode) {
            fprintf(stderr, "FT_Load_Char failed code: %d character: %lu\n", errorCode,c);
            goto CLEANUP_FONT_MANAGER;
        }

        slot = loadingFace->glyph;
        bmp = slot->bitmap;

        glyph_width = bmp.width;
        glyph_height = bmp.rows;

        if (glyph_width > segment_size_x) {
            segment_size_x = glyph_width;
        }

        if (glyph_height > segment_size_y) {
            segment_size_y = glyph_height;
        }
    }

    font_tex_width = nextp2(num_segments_x * segment_size_x);
    font_tex_height = nextp2(num_segments_y * segment_size_y);

    if ( font_tex_height > MAX_WIDTH_HEIGHT) {
        ALOGE("ERROR cannot allocate a height of: %d, max texture size: %d",font_tex_height,MAX_WIDTH_HEIGHT);
    }

    if ( font_tex_width > MAX_WIDTH_HEIGHT) {
        ALOGE("ERROR cannot allocate a width of: %d, max texture size: %d",font_tex_width,MAX_WIDTH_HEIGHT);
    }

    ALOGI("TEXTURE size: %dx%d",font_tex_width,font_tex_height);
    font_texture_data = (GLubyte*) new GLubyte[font_tex_width * font_tex_height];

    for(FT_ULong c = 0; c < 128; c++) {
        errorCode = FT_Load_Char(loadingFace, c, FT_LOAD_RENDER);
        if(FT_Load_Char(loadingFace, c, FT_LOAD_RENDER)) {
            fprintf(stderr, "FT_Load_Char failed code: %d character: %lu\n",errorCode,c);
            goto CLEANUP_FONT_MANAGER;
        }

        slot = loadingFace->glyph;
        bmp = slot->bitmap;

        glyph_width = nextp2(bmp.width);
        glyph_height = nextp2(bmp.rows);

        div_t temp = div(c, num_segments_x);

        bitmap_offset_x = segment_size_x * temp.rem;
        bitmap_offset_y = segment_size_y * temp.quot;

        for (int j = 0; j < glyph_height; j++) {
            for (int i = 0; i < glyph_width; i++) {
                GLubyte glyphByte = GLubyte((i >= bmp.width || j >= bmp.rows)? 0 : bmp.buffer[i + bmp.width * j]);
                int bitmap_index = (bitmap_offset_x + i) + (j + bitmap_offset_y) * font_tex_width;
                font_texture_data[bitmap_index] = glyphByte;

            }
        }

        font->advance[c] = (float)(slot->advance.x >> 6);
        font->tex_x1[c] = (float)bitmap_offset_x / (float) font_tex_width;
        font->tex_x2[c] = (float)(bitmap_offset_x + bmp.width) / (float)font_tex_width;
        font->tex_y1[c] = (float)bitmap_offset_y / (float) font_tex_height;
        font->tex_y2[c] = (float)(bitmap_offset_y + bmp.rows) / (float)font_tex_height;
        font->width[c] = bmp.width;
        font->height[c] = bmp.rows;
        font->offset_x[c] = (float)slot->bitmap_left;
        font->offset_y[c] =  (float)((slot->metrics.horiBearingY-loadingFace->glyph->metrics.height) >> 6);
    }

    checkGlError("glGenTextures[font_texture]before");
    glGenTextures(1, &font->font_texture);
    checkGlError("glGenTextures[font_texture]");

    glBindTexture(GL_TEXTURE_2D, font->font_texture);
    checkGlError("glTexParameteri[glBindTexture]");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    checkGlError("glTexParameteri[GL_TEXTURE_MAG_FILTER]");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    checkGlError("glTexParameteri[GL_TEXTURE_MIN_FILTER]");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    checkGlError("glTexParameteri[GL_TEXTURE_WRAP_S]");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    checkGlError("glTexParameteri[GL_TEXTURE_WRAP_T]");

    glBindTexture(GL_TEXTURE_2D, font->font_texture);
    checkGlError("glBindTexture");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    checkGlError("glTexParameteri");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, font_tex_width, font_tex_height, 0, GL_ALPHA , GL_UNSIGNED_BYTE, font_texture_data);
    checkGlError("attribTexture->glTexImage2D");
    delete [] font_texture_data;
    font_texture_data = NULL;
    FT_Done_Face(loadingFace);
    transform(fontname.begin(), fontname.end(), fontname.begin(), ::tolower);
    font->name = fontname;

    return font;

CLEANUP_FONT_MANAGER:
    delete [] font_texture_data;
    FT_Done_Face(loadingFace);
    delete font;
    return NULL;
}

meow_text * FontManager::load_text(const char* msg,const meow_font* font)const {
    int i, c;
    GLshort* indices;
    int number_of_verts = strlen(msg) * 4;
    meow_vertex_buffer * vertex_attributes = new meow_vertex_buffer[number_of_verts];
    float pen_x = 0.0f;
    float lineheight = 0.0f;

    meow_text * text_object = new meow_text();

    if (!font) {
        ALOGE("Font must not be null\n");
        return NULL;
    }

    if (font->initialized == -1) {
        ALOGE("Font has not been loaded\n");
        return NULL;
    }

    if (!msg) {
        return NULL;
    }
    size_t index_size = strlen(msg);
    indices = new GLshort[6 * index_size];
    memset(indices, 0x00, sizeof(GLshort)*6*index_size);
    for(i = 0; i < index_size; ++i) {
        c = msg[i];

        vertex_attributes[4 * i].pos[0] = pen_x + font->offset_x[c];
        vertex_attributes[4 * i].pos[1] = font->offset_y[c];
        vertex_attributes[4 * i + 1].pos[0] = vertex_attributes[4 * i].pos[0] + font->width[c];
        vertex_attributes[4 * i + 1].pos[1] = vertex_attributes[4 * i].pos[1];
        vertex_attributes[4 * i + 2].pos[0] = vertex_attributes[4 * i].pos[0];
        vertex_attributes[4 * i + 2].pos[1] = vertex_attributes[4 * i].pos[1] + font->height[c];
        vertex_attributes[4 * i + 3].pos[0] = vertex_attributes[4 * i + 1].pos[0];
        vertex_attributes[4 * i + 3].pos[1] = vertex_attributes[4 * i + 2].pos[1];

        vertex_attributes[4 * i].tex[0] = font->tex_x1[c];
        vertex_attributes[4 * i].tex[1] = font->tex_y2[c];
        vertex_attributes[4 * i + 1].tex[0] = font->tex_x2[c];
        vertex_attributes[4 * i + 1].tex[1] = font->tex_y2[c];
        vertex_attributes[4 * i + 2].tex[0] = font->tex_x1[c];
        vertex_attributes[4 * i + 2].tex[1] = font->tex_y1[c];
        vertex_attributes[4 * i + 3].tex[0] = font->tex_x2[c];
        vertex_attributes[4 * i + 3].tex[1] = font->tex_y1[c];

        indices[i * 6 + 0] = GLshort (4 * i + 0);
        indices[i * 6 + 1] = GLshort (4 * i + 1);
        indices[i * 6 + 2] = GLshort (4 * i + 2);
        indices[i * 6 + 3] = GLshort (4 * i + 2);
        indices[i * 6 + 4] = GLshort (4 * i + 1);
        indices[i * 6 + 5] = GLshort (4 * i + 3);


        /* Assume we are only working with typewriter fonts */
        pen_x += font->advance[c];
        if ( vertex_attributes[4 * i].pos[1] + font->height[c] > lineheight) {
            lineheight = vertex_attributes[4 * i].pos[1] + font->height[c];
        }
    }

    text_object->width = pen_x;
    text_object->height = lineheight;
    text_object->vertecies = strlen(msg) * 6;

    glGenBuffers(2, &text_object->font_index_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, text_object->font_vertex_data);
    checkGlError("GL_ARRAY_BUFFER->glBufferData");
    glBufferData(GL_ARRAY_BUFFER,sizeof(meow_vertex_buffer) * number_of_verts,(GLvoid *) vertex_attributes, GL_STATIC_DRAW);
    checkGlError("GL_ARRAY_BUFFER->glBufferData");
    delete [] vertex_attributes;
    vertex_attributes = NULL;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, text_object->font_index_buffer);
    checkGlError("GL_ELEMENT_ARRAY_BUFFER->glBindBuffer");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLushort) * text_object->vertecies,(GLvoid *) indices, GL_STATIC_DRAW);
    checkGlError("GL_ELEMENT_ARRAY_BUFFER->glBufferData");
    delete [] indices;
    indices = NULL;

    text_object->font = font;

    return text_object;
}

void FontManager::drawText(const char * msg,meow_font * font, TextAlignment alignmentFlags, glm::vec4 color, int32_t offsetX, int32_t offsetY) {
    draw(getText(msg,font),alignmentFlags, color, offsetX, offsetY);
}

meow_text *FontManager::get_text(const char *msg, const meow_font *font) {
    return texts.get(get_key(msg,font));
}

string FontManager::get_key(const char *msg, const meow_font *font) {
    ostringstream s;
    s << msg << (char) ':' << font->initialized;
    return s.str();
}

void FontManager::draw(meow_text *text,TextAlignment align, glm::vec4 color, int32_t offsetx, int32_t offsety) {
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    //glDepthFunc(GL_NEVER);

    glUseProgram(shaderManager->getShaderProgram(textShader));
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glm::mat4 orthographicProjection = glm::ortho((float)-screenWidth/2.0f,(float) screenWidth/2.0f,(float)-screenHeight/2.0f,(float) screenHeight/2.0f,-1.0f,1.0f);
    glm::mat4 modelviewMatix = getModelView(text,align,offsetx,offsety);

    if (shaderManager->hasUniform(textShader,SHADER_UNIFORM_MODEL_MATRIX)) {
        glUniformMatrix4fv(shaderManager->getUniform(textShader, SHADER_UNIFORM_MODEL_MATRIX), 1,
                           GL_FALSE,
                           glm::value_ptr(modelviewMatix));
    } else {
        //ALOGE("YOU SHOULD HAVE A MODELVIEW MATRIX!!");
    }

    if (shaderManager->hasUniform(textShader,SHADER_UNIFORM_PROJECTION_MATRIX)) {
        glUniformMatrix4fv(shaderManager->getUniform(textShader, SHADER_UNIFORM_PROJECTION_MATRIX), 1,
                           GL_FALSE,
                           glm::value_ptr(orthographicProjection));
    } else {
        //ALOGE("YOU SHOULD HAVE A PROJECTION MATRIX!!");
    }

    if (shaderManager->hasUniform(textShader,SHADER_UNIFORM_COLOR)) {
        glUniform4fv(shaderManager->getUniform(textShader,SHADER_UNIFORM_COLOR), 1, glm::value_ptr(color));
    } else {
        //ALOGE("YOU SHOULD HAVE A COLOR MATRIX!!");
    }

    glActiveTexture(GL_TEXTURE0);
    checkGlError("glActiveTexture");
    glBindTexture(GL_TEXTURE_2D, text->font->font_texture);
    checkGlError("glBindTexture");

    if (shaderManager->hasUniform(textShader,SHADER_UNIFORM_TEXTURE_ID)) {
        glUniform1i(shaderManager->getUniform(textShader, SHADER_UNIFORM_TEXTURE_ID),0);
    }

    GLuint attribPosition = (GLuint) shaderManager->getAttrib(textShader,SHADER_ATTRIBUTE_POSITION);
    GLuint attribTexture  = (GLuint) shaderManager->getAttrib(textShader,SHADER_ATTRIBUTE_TEX_COORD);

    glBindBuffer(GL_ARRAY_BUFFER, text->font_vertex_data);
    glEnableVertexAttribArray(attribTexture);
    checkGlError("[fontManager]attribTexture->glEnableVertexAttribArray");
    glVertexAttribPointer(attribTexture, 2, GL_FLOAT, GL_FALSE, sizeof(meow_vertex_buffer),(GLvoid *) offsetof(struct meow_vertex_buffer,tex));
    checkGlError("[fontManager]attribTexture->glVertexAttribPointer");

    glEnableVertexAttribArray(attribPosition);
    checkGlError("[fontManager]attribPosition->glEnableVertexAttribArray");
    glVertexAttribPointer(attribPosition, 2, GL_FLOAT, GL_FALSE, sizeof(meow_vertex_buffer), (GLvoid *) offsetof(struct meow_vertex_buffer,pos));
    checkGlError("[fontManager]attribPosition->glVertexAttribPointer");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, text->font_index_buffer);
    checkGlError("[fontManager]bindBuffer");
    glDrawElements(GL_TRIANGLES, text->vertecies, GL_UNSIGNED_SHORT, NULL);
    checkGlError("[fontManager]glDrawElements");

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void FontManager::drawText(const char *msg, int32_t fontId) {
    drawText(msg,getFont(fontId));
}

void FontManager::  drawText(const char *msg, TextAlignment alignmentFlags, glm::vec4 color, int32_t offsetX, int32_t offsetY, int32_t fontId) {
    drawText(msg,getFont(fontId),alignmentFlags, color, offsetX,offsetY);
}

meow_font *FontManager::getFont(int32_t fontIndex) {
    return weak_ref[fontIndex];
}

float FontManager::getHorizontalPosition(TextAlignment alignmentFlags,const meow_text * text) const {
    GLfloat xPos = 0.0f;

    if (alignmentFlags & TextAlignLeft) {
        xPos = -screenWidth / 2.0f;
    } else if (alignmentFlags & TextAlignRight) {
        xPos = screenWidth / 2.0f - text->width;
    } else if (alignmentFlags & TextAlignCenterHorizontally) {
        xPos = -text->width /2.0f;
    }

    return xPos;
}

float FontManager::getVerticalPosition(TextAlignment alignmentFlags,const meow_text * text) const {
    GLfloat yPos = 0.0f;

    if (alignmentFlags & TextAlignBottom) {
        yPos =  -screenHeight / 2.0f;
    } else if (alignmentFlags & TextAlignTop) {
        yPos = screenHeight / 2.0f - text->height;
    } else if (alignmentFlags & TextAlignCenterVertically) {
        yPos = -text->height /2.0f;
    }

    return yPos;
}

float FontManager::getVerticalPosition(TextAlignment alignmentFlags,const char * text, int font) {
    return getVerticalPosition(alignmentFlags,getText(text,getFont(font)));
}

float FontManager::getHorizontalPosition(TextAlignment alignmentFlags,const char * text, int font) {
    return getHorizontalPosition(alignmentFlags,getText(text,getFont(font)));
}

glm::mat4 FontManager::getModelView(const meow_text * text, TextAlignment alignmentFlags, int32_t offsetX,
                                    int32_t offsetY) const {
    GLfloat yPos = getVerticalPosition(alignmentFlags,text);
    GLfloat xPos = getHorizontalPosition(alignmentFlags,text);

    xPos += offsetX;
    yPos += offsetY;

    return glm::translate(glm::vec3(xPos,yPos,0.0f));
}

meow_text * FontManager::getText(const char *msg, meow_font * font) {
    meow_text * text = get_text(msg,font);
    if (text == NULL) {
        text = load_text(msg,font);
        texts.put(get_key(msg,font),text);
    }
    return text;
}

int FontManager::getFontFromName(string name) {
    transform(name.begin(), name.end(), name.begin(), ::tolower);
    std::map<string,int>::iterator fontForName = font_names.find(name);
    if ( fontForName != font_names.end()) {
        return fontForName->second;
    }

    return -1;
}

float FontManager::dpiToPixels(int32_t dpi) {
    return dpi * (screenDensity / 160.0f);
}

float FontManager::devicePixelRatio() const {
    return (screenDensity / 160.0f);
}
