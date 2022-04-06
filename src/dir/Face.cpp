//
// Created by wzw on 2022/3/21.
//

#include "Face.h"

Face::Face(FT_Face face, std::string font, unsigned int width, unsigned int height) {
    this->mFace = face;
    this->mFontFile = std::move(font);
    this->mWidth = width;
    this->mHeight = height;
}

FT_Face& Face::getFace() {
    return mFace;
}

unsigned int Face::getWidth() const {
    return mWidth;
}

unsigned int Face::getHeight() const {
    return mHeight;
}


void Face::destroy() {
    mFontFile.clear();
    mWidth = 0;
    mHeight = 0;
    FT_Done_Face(mFace);
}
