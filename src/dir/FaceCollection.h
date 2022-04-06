//
// Created by wzw on 2022/3/21.
//

#ifndef OPENGL_FACECOLLECTION_H
#define OPENGL_FACECOLLECTION_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_LCD_FILTER_H
#include FT_IMAGE_H
#include FT_OUTLINE_H

#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb.h>

#include <cassert>
#include <vector>

#include "Face.h"
#include "ErrorCode.h"
#include "Text.h"
#include "Character.h"

class FaceCollection {
private:

    // 从上向下优先级越来越低，可能需要动态不同 face 的位置来调整优先级
    /*
     * font face 的集合
     */
    std::vector<Face> mFaces;

    std::vector<std::string> mFontFiles;

    FT_Library ft{};

    hb_buffer_t *buffer{};

    unsigned int mFontPixelWidth{};

    unsigned int mFontPixelHeight{};

    ErrorCode mErrorCode = ErrorCode::ok;

public:

    FaceCollection() = default;

    FaceCollection(std::vector<std::string> fontFiles, unsigned int fontPixelWidth, unsigned int fontPixelHeight);

    ~FaceCollection();

    /**
     * 加载所有 face 并设置 size 等
     */
    void loadFaces();

    /**
     * 根据 face 获取字符的 glyph index
     * 填充 text 中的 shape cache
     *
     * @param text  text obj
     */
    void assignCodepointsFaces(Text &text);

    /**
     * 根据 face index，glyph index 获取 bitmap 填充 Character
     *
     * @param faceIndex       mFaces 集合下标
     * @param glyphIndex      字形 id
     * @return                character
     */
    void renderGlyph(Text& text, Character& character, const FT_Face& face, unsigned int glyphIndex);

    // getter and setter
    std::vector<Face>& getFaces();

    std::vector<Face>& getEmojiFaces();

    std::vector<std::string> getFontFiles() const;

    unsigned int getMFontPixelWidth() const;

    unsigned int getMFontPixelHeight() const;

    ErrorCode getErrorCode() const;

private:

    std::vector<unsigned int> getUTF8(std::string line);

};


#endif //OPENGL_FACECOLLECTION_H
