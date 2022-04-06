//
// Created by wzw on 2022/3/21.
//

#include "FaceCollection.h"
#include "Face.h"
#include "Util.h"
#include <cstdlib>



//std::vector<unsigned int> FaceCollection::getUTF8(std::string line) {
//    hb_buffer_clear_contents(buffer);
//    hb_buffer_guess_segment_properties()
//}



FaceCollection::FaceCollection(std::vector<std::string> fontFiles, unsigned int fontPixelWidth, unsigned int fontPixelHeight) {
    if (FT_Init_FreeType(&ft)) {
        mErrorCode = ErrorCode::init_ft_error;
        fprintf(stderr, "Could not init ft library\n");
        exit(EXIT_FAILURE);
    }
    assert(mErrorCode == ErrorCode::ok);
    // Set which filter to use for the LCD (liquid crystal display) Subpixel Antialiasing
    FT_Library_SetLcdFilter(ft, FT_LCD_FILTER_DEFAULT);
    this->mFontFiles = std::move(fontFiles);
    this->mFontPixelWidth = fontPixelWidth;
    this->mFontPixelHeight = fontPixelHeight;

    buffer = hb_buffer_create();
}

FaceCollection::~FaceCollection() {
    for (auto& face : mFaces) {
        face.destroy();
    }
    for (auto& face : mEmojiFaces) {
        face.destroy();
    }

    FT_Done_FreeType(ft);
    hb_buffer_destroy(buffer);
}


void FaceCollection::loadFaces() {
    assert(!mFontFiles.empty());
    for (const auto& fontFile : mFontFiles) {
        FT_Face face;
        if (FT_New_Face(ft, fontFile.c_str(), 0, &face)) {
            fprintf(stderr, "Could not load font\n");
            exit(EXIT_FAILURE);
        }

        /**
         * TODO: 是否可以根据 FT_HAS_COLOR 来区分 emoji face ? bug ?
         */
        if (FT_HAS_COLOR(face)) {
            if (FT_Select_Size(face, 0)) {
                fprintf(stderr, "Could not request the font size (fixed)\n");
                exit(EXIT_FAILURE);
            }
        }
        else {
            if (FT_Set_Pixel_Sizes(face, mFontPixelWidth, mFontPixelHeight)) {
                fprintf(stderr, "Could not request the font size (in pixels)\n");
                exit(EXIT_FAILURE);
            }
        }

        // The mFace's size and bbox are populated only after set pixel
        // sizes/select size have been called
        unsigned int width;
        unsigned int height;
        if (FT_IS_SCALABLE(face)) {

            // TODO: bbox.xMin = FLOOR( bbox.xMin )
            //  bbox.yMin = FLOOR( bbox.yMin )
            //  bbox.xMax = CEILING( bbox.xMax )
            //  bbox.yMax = CEILING( bbox.yMax )
            /*
             * Note that the bounding box might be off by (at least) one pixel for hinted fonts.
             * See FT_Size_Metrics for further discussion.
             */

            /*
             * You can scale a distance expressed in font units to 26.6 pixel format directly with the help of the FT_MulFix function
             * convert design distances to 1/64th of pixels   ->   >> 6   to pixels
             */
            width = FT_MulFix(face->bbox.xMax - face->bbox.xMin, face->size->metrics.x_scale) >> 6;
            height = FT_MulFix(face->bbox.yMax - face->bbox.yMin, face->size->metrics.y_scale) >> 6;
        }
        else {
            width = face->available_sizes[0].width;
            height = face->available_sizes[0].height;
        }

        Face faceL(face, fontFile, width, height);
        // 只在此处 new FontFace
        if (FT_HAS_COLOR(face)) {
            mEmojiFaces.push_back(faceL);
        }
        else {
            mFaces.push_back(faceL);
        }
    }
}


void FaceCollection::assignCodepointsFaces(Text &text) {
    assert(!mFaces.empty() && !mEmojiFaces.empty());
    std::vector<std::string> lines = text.getLines();
    assert(!lines.empty());

    // 缺失的 face 和 glyph 的标记
    const unsigned int MISSING_FLAG = UINT32_MAX;

    for (const auto& line : lines) {
        if (text.isShapeCacheContainsLine(line)) {
            continue;
        }

        // 获取该行的 utf8 编码，也就是 codepoint, 根据 codepoint 分段



        // 记录 emoji 的 index
        std::vector<unsigned int> emojiIndexes;
        // 记录 character 的 index
        std::vector<unsigned int> charIndexes;
        std::vector<ShapeCacheElement> lineGlyphCache;

        bool isAllCharacterHasFace = false;
        for (unsigned int i = 0; (i < mFaces.size()) && !isAllCharacterHasFace; i++) {
            isAllCharacterHasFace = true;

            hb_buffer_clear_contents(buffer);

            hb_buffer_add_utf8(buffer, line.c_str(), (int)line.length(), 0, -1);

            /*
             * HarfBuzz also provides getter functions to retrieve a buffer's direction, script, and language properties individually.
             * HarfBuzz recognizes four text directions in hb_direction_t: left-to-right (HB_DIRECTION_LTR), right-to-left (HB_DIRECTION_RTL), top-to-bottom (HB_DIRECTION_TTB), and bottom-to-top (HB_DIRECTION_BTT).
             * For the script property, HarfBuzz uses identifiers based on the ISO 15924 standard. For languages, HarfBuzz uses tags based on the IETF BCP 47 standard.
             * Helper functions are provided to convert character strings into the necessary script and language tag types.
             */

            hb_buffer_guess_segment_properties(buffer);

            FT_Face face = mFaces[i].getFace();
            hb_font_t *font = hb_ft_font_create(face, nullptr);

            /*
             * 初始化
             */
            if (i == 0) {
                unsigned int _glyphCount;
                hb_glyph_info_t *_glyphInfo = hb_buffer_get_glyph_infos(buffer, &_glyphCount);
                lineGlyphCache.resize(_glyphCount);

                std::cout << "line.size(): " << line.size() << std::endl;
                for (unsigned int k = 0; k < _glyphCount; k++) {
                    bool isEmoji = Util::isEmojiCharacter(_glyphInfo[k].codepoint);

                    std::cout << k << ":   codepoint: " << std::hex << _glyphInfo[k].codepoint << std::endl;



                    if (isEmoji) {
                        emojiIndexes.push_back(k);
                    }
                    else {
                        charIndexes.push_back(k);
                    }
                    lineGlyphCache[k].isEmoji = isEmoji;
                    lineGlyphCache[k].codepoint = _glyphInfo[k].codepoint;
                    lineGlyphCache[k].faceIndex = MISSING_FLAG;
                    lineGlyphCache[k].glyphIndex = MISSING_FLAG;
                }
            }

            std::vector<hb_feature_t> features(3);
            assert(hb_feature_from_string("kern=1", -1, &features[0]));
            assert(hb_feature_from_string("liga=1", -1, &features[1]));
            assert(hb_feature_from_string("clig=1", -1, &features[2]));
            // shape the font
            hb_shape(font, buffer, &features[0], features.size());

            // get the glyph information
            unsigned int glyphCount;
            hb_glyph_info_t *glyphInfo = hb_buffer_get_glyph_infos(buffer, &glyphCount);
            for (unsigned int index : charIndexes) {
                hb_codepoint_t glyphIndex = glyphInfo[index].codepoint;
                if (glyphIndex != 0 && lineGlyphCache[index].faceIndex == MISSING_FLAG) {
                    lineGlyphCache[index].isEmoji = false;
                    lineGlyphCache[index].faceIndex = i;
                    lineGlyphCache[index].glyphIndex = glyphIndex;
                }
                else if (glyphIndex == 0 && lineGlyphCache[index].glyphIndex == MISSING_FLAG) {
                    isAllCharacterHasFace = false;
                }
            }

            hb_font_destroy(font);
        }



        /**
         * 处理行中的 emoji
         */
        bool isAllEmojiHasFace = false;
        if (!emojiIndexes.empty()) {
            for (unsigned int i = 0; (i < mEmojiFaces.size()) && !isAllEmojiHasFace; i++) {
                isAllEmojiHasFace = true;

                hb_buffer_clear_contents(buffer);
                hb_buffer_add_utf8(buffer, line.c_str(), (int)line.length(), 0, -1);
                hb_buffer_guess_segment_properties(buffer);

                FT_Face face;
                face = mEmojiFaces[i].getFace();
                hb_font_t *font = hb_ft_font_create(face, nullptr);

                hb_shape(font, buffer, nullptr, 0);
                unsigned int glyphCount;
                hb_glyph_info_t *glyphInfo = hb_buffer_get_glyph_infos(buffer, &glyphCount);

                for (unsigned int index : emojiIndexes) {
                    hb_codepoint_t glyphIndex = glyphInfo[index].codepoint;
                    if (glyphIndex != 0 && lineGlyphCache[index].faceIndex == MISSING_FLAG) {
                        lineGlyphCache[index].isEmoji = true;
                        lineGlyphCache[index].faceIndex = i;
                        lineGlyphCache[index].glyphIndex = glyphIndex;
                    }
                    else if (glyphIndex == 0 && lineGlyphCache[index].glyphIndex == MISSING_FLAG) {
                        isAllCharacterHasFace = false;
                    }
                }

                hb_font_destroy(font);
            }
        }


        /**
         * TODO: 找不到 glyph 时这点有问题
         */
         if ((!charIndexes.empty() && !isAllCharacterHasFace) || (!emojiIndexes.empty() && !isAllEmojiHasFace)) {
             for (auto& ele : lineGlyphCache) {
                 if (ele.faceIndex == MISSING_FLAG && ele.glyphIndex == MISSING_FLAG) {
                     const auto REPLACEMENT_CHARACTER = 0x0000FFFD;
                     ele.faceIndex = 0;
                     // 当本 face 中没有 该char 的字形时， 有可能崩溃，修改
                     ele.glyphIndex = FT_Get_Char_Index(mFaces[0].getFace(), REPLACEMENT_CHARACTER);
                 }
             }
         }

        text.insertShapeCache(line, lineGlyphCache);
    }

}


void FaceCollection::renderGlyph(Text& text, Character& character, const FT_Face& face, unsigned int glyphIndex) {

    bool hasColor = FT_HAS_COLOR(face);

    FT_Int32 flags = FT_LOAD_DEFAULT | FT_LOAD_TARGET_LCD;
    if (hasColor) {
        flags |= FT_LOAD_COLOR;
    }

    if (FT_Load_Glyph(face, glyphIndex, flags)) {
        fprintf(stderr, "Could not load glyph with glyph index: %u\n", glyphIndex);
        exit(EXIT_FAILURE);
    }

    // TODO: bold ?
    /**
     * Embolden an outline. The new outline will be at most 4 times strength pixels wider and higher.
     * You may think of the left and bottom borders as unchanged.
     * Negative strength values to reduce the outline thickness are possible also.
     */
    if (text.getIsBold()) {
        int strength = 1 << 6;
        if (face->glyph->format == FT_GLYPH_FORMAT_OUTLINE) {
            if (FT_Outline_Embolden(&face->glyph->outline, strength)) {
                fprintf(stderr, "Could not embolden glyph with strength: %u\n", glyphIndex);
                exit(EXIT_FAILURE);
            }
        }
    }

    // TODO: italic
    /**
     *     x | xx xy    x'    x' = x * xx + y * xy
     *     y | yx yy    y'    y' = x * yx + y * yy
     *     Note that the 2×2 transformation matrix is always applied to the 16.16 advance vector in the glyph;
     *     you thus don't need to recompute it
     *
     *     so:
     *     1   0.58
     *     0   1
     */
    if (text.getIsItalic()) {
        FT_Matrix matrix;
        matrix.xx = 1 << 16;
        matrix.xy = (long)(text.getAngle() * 0x10000L);
        matrix.yx = 0;
        matrix.yy = 1 << 16;
        FT_Outline_Transform(&face->glyph->outline, &matrix);
    }


    /**
     * TODO: 顺时针旋转, 针对不同的 字符进行旋转，
     *  像 汉字，日语，韩语等不需要旋转
     *  像 英语，俄语，等单词样式的得进行旋转
     */
//    if (text.getIsVertical()) {
//        double arg = 90;
//        double angel = (arg / 360.0) * 3.14159 * 2;
//        FT_Matrix matrix;
//        matrix.xx = (FT_Fixed)(std::cos(angel) * 0x10000L);
//        matrix.xy = (FT_Fixed)(std::sin(angel) * 0x10000L);
//        matrix.yx = (FT_Fixed)(-std::sin(angel) * 0x10000L);
//        matrix.yy = (FT_Fixed)(std::cos(angel) * 0x10000L);
//        FT_Outline_Transform(&face->glyph->outline, &matrix);
//    }


    if (!hasColor) {
        if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_LCD)) {
            fprintf(stderr, "Could not render glyph with glyph index: %u\n", glyphIndex);
            exit(EXIT_FAILURE);
        }
    }

    std::vector<unsigned char>& bitmapBuffer = character.getBitmapBuffer();


    /*
     * face->glyph->bitmap.width
     */
    if (!hasColor) {
        bitmapBuffer.resize(face->glyph->bitmap.width * face->glyph->bitmap.rows * 3);
        for (unsigned int i = 0; i < face->glyph->bitmap.rows; i++) {
            for (unsigned int j = 0; j < face->glyph->bitmap.width; j++) {
                unsigned char ch = face->glyph->bitmap.buffer[i * face->glyph->bitmap.pitch + j];
                bitmapBuffer[i * face->glyph->bitmap.width + j] = ch;
            }
        }
    }
    else {
        bitmapBuffer.resize(face->glyph->bitmap.width * face->glyph->bitmap.rows * 4);
        std::copy(face->glyph->bitmap.buffer,
                  face->glyph->bitmap.buffer + face->glyph->bitmap.rows * face->glyph->bitmap.width * 4,
                  bitmapBuffer.begin());
    }

    if (!hasColor) {
        character.setSize(face->glyph->bitmap.width / 3, face->glyph->bitmap.rows);
    }
    else {
        character.setSize(face->glyph->bitmap.width, face->glyph->bitmap.rows);
    }

    character.setBearing(face->glyph->bitmap_left, face->glyph->bitmap_top);
    character.setAdvance(face->glyph->advance.x);
    character.setColor(hasColor);
}


std::vector<Face>& FaceCollection::getFaces() {
    return mFaces;
}

std::vector<Face>& FaceCollection::getEmojiFaces() {
    return mEmojiFaces;
}


std::vector<std::string> FaceCollection::getFontFiles() const {
    return mFontFiles;
}

unsigned int FaceCollection::getMFontPixelWidth() const {
    return mFontPixelWidth;
}

unsigned int FaceCollection::getMFontPixelHeight() const {
    return mFontPixelHeight;
}

ErrorCode FaceCollection::getErrorCode() const {
    return mErrorCode;
}
















