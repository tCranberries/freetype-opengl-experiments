//
// Created by wzw on 2022/3/21.
//

#ifndef OPENGL_FACE_H
#define OPENGL_FACE_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <vector>
#include <string>

/**
 * font -> face
 */
class Face {
private:
    std::string mFontFile;

    FT_Face mFace{};

    unsigned int mWidth{};

    unsigned int mHeight{};

public:
    Face() = default;

    Face(FT_Face face, std::string font, unsigned int width, unsigned int height);

    FT_Face& getFace();

    unsigned int getWidth() const;

    unsigned int getHeight() const;

    void destroy();
};


#endif //OPENGL_FACE_H
