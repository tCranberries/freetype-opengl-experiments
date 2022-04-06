// Copyright 2019 <Andrea Cognolato>
#include "./texture_atlas.h"

namespace texture_atlas {
    static GLsizei kTextureDepth = 1024;
    static GLsizei kMipLevelCount = 1;

    TextureAtlas::TextureAtlas(GLsizei textureWidth,
                               GLsizei textureHeight,
                               GLuint shaderProgramId,
                               const char* textureUniformLocation,
                               GLenum internalformat, GLenum format,
                               GLint shader_texture_index)
            : textureWidth_(textureWidth),
              textureHeight_(textureHeight),
              //texture_cache_(kTextureDepth),
              format_(format) {
        // https://www.cnblogs.com/psklf/p/5713791.html
        glGenTextures(1, &texture_);
        // Array Texture这个东西的意思是，一个纹理对象，可以存储不止一张图片信息，就是说是是一个数组，每个元素都是一张图片。这样免了频繁地去切换当前需要bind的纹理
        glBindTexture(GL_TEXTURE_2D_ARRAY, texture_);
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, kMipLevelCount, internalformat,
                       textureWidth_, textureHeight_, kTextureDepth);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

        glUniform1i(glGetUniformLocation(shaderProgramId, textureUniformLocation),
                    shader_texture_index);
    }
    TextureAtlas::~TextureAtlas() { glDeleteTextures(1, &texture_); }

    /**
     * Character
     *
     * @param bitmap_buffer    character bitmap buffer
     * @param width            character size x ->
     * @param height
     * @param ch
     * @param offset
     */
//    void TextureAtlas::Insert(const vector<unsigned char>& bitmap_buffer, GLsizei width, GLsizei height, Character* ch, GLuint offset) {
//        assert(static_cast<GLsizei>(offset) < kTextureDepth);
//
//        GLint level = 0, xOffset = 0, yOffset = 0;
//        GLsizei depth = 1;
//
//        glBindTexture(GL_TEXTURE_2D_ARRAY, texture_);
//        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, level, xOffset, yOffset, (int)offset, width,
//                        height, depth, format_, GL_UNSIGNED_BYTE,
//                        bitmap_buffer.data());
//        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
//
//        /*
//         * 获取根源 RenderGlyph
//         * mWidth = (int)mFace->glyph->bitmap.mWidth / 3;
//         * mHeight = (int)mFace->glyph->bitmap.rows;
//         *
//         * 获取根源 LoadFaces
//         *  if (FT_IS_SCALABLE(mFace)) {
//                textureWidth_ = (int)FT_MulFix(mFace->bbox.xMax - mFace->bbox.xMin,mFace->size->metrics.x_scale) >> 6;
//                textureHeight_ = (int)FT_MulFix(mFace->bbox.yMax - mFace->bbox.yMin,mFace->size->metrics.y_scale) >> 6;
//            } else {
//                textureWidth_ = (mFace->available_sizes[0].mWidth);
//                textureHeight_ = (mFace->available_sizes[0].mHeight);
//            }
//         */
//        auto v = glm::vec2((float)width / static_cast<GLfloat>(textureWidth_),
//                           (float)height / static_cast<GLfloat>(textureHeight_));
//        ch->texture_id = texture_;
//        ch->texture_array_index = offset;
//        ch->texture_coordinates = v;
//    }

    void TextureAtlas::Insert(Character& ch) {
        unsigned int offset = index_++;

        glm::vec2 size = ch.getSize();
        std::vector<unsigned char> bitmap_buffer = ch.getBitmapBuffer();

        int width = (int)size.x;
        int height = (int)size.y;



        assert(static_cast<GLsizei>(offset) < kTextureDepth);

        GLint level = 0, xOffset = 0, yOffset = 0;
        GLsizei depth = 1;

        glBindTexture(GL_TEXTURE_2D_ARRAY, texture_);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, level, xOffset, yOffset, (int)offset, width,
                        height, depth, format_, GL_UNSIGNED_BYTE,
                        bitmap_buffer.data());
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

        /*
         * 获取根源 RenderGlyph
         * mWidth = (int)mFace->glyph->bitmap.mWidth / 3;
         * mHeight = (int)mFace->glyph->bitmap.rows;
         *
         * 获取根源 LoadFaces
         *  if (FT_IS_SCALABLE(mFace)) {
                textureWidth_ = (int)FT_MulFix(mFace->bbox.xMax - mFace->bbox.xMin,mFace->size->metrics.x_scale) >> 6;
                textureHeight_ = (int)FT_MulFix(mFace->bbox.yMax - mFace->bbox.yMin,mFace->size->metrics.y_scale) >> 6;
            } else {
                textureWidth_ = (mFace->available_sizes[0].mWidth);
                textureHeight_ = (mFace->available_sizes[0].mHeight);
            }
         */
        auto v = glm::vec2((float)width / static_cast<GLfloat>(textureWidth_),
                           (float)height / static_cast<GLfloat>(textureHeight_));
        ch.setTextureId(texture_);
        ch.setTextureArrayIndex(offset);
        ch.setTextureCoordinates(v);
    }

//    void TextureAtlas::Append(std::pair<Character, vector<unsigned char>>* p, hb_codepoint_t glyphIndex) {
//        Insert(p->second, p->first.size.x, p->first.size.y, &p->first, index_++);
//
//        auto& item = texture_cache_[glyphIndex];
//        item.character = p->first;
//        item.fresh = true;
//    }
//
//    void TextureAtlas::Replace(std::pair<Character, vector<unsigned char>>* p, hb_codepoint_t firstStaleIndex, hb_codepoint_t glyphIndex) {
//        Insert(p->second, p->first.size.x, p->first.size.y, &p->first,
//               texture_cache_[firstStaleIndex].character.texture_array_index);
//
//        texture_cache_.erase(firstStaleIndex);
//
//        auto& item = texture_cache_[glyphIndex];
//        item.character = p->first;
//        item.fresh = true;
//    }
//
//    bool TextureAtlas::Contains(hb_codepoint_t codepoint) const {
//        return texture_cache_.find(codepoint) != texture_cache_.end();
//    }
//
//    Character* TextureAtlas::Get(hb_codepoint_t codepoint) {
//        auto it = texture_cache_.find(codepoint);
//        if (it != texture_cache_.end()) {
//            it->second.fresh = true;
//            return &(it->second.character);
//        }
//        return nullptr;
//    }
//
//    void TextureAtlas::Insert(hb_codepoint_t glyphIndex, pair<Character, vector<unsigned char>>* ch) {
//        // 缓存没有满    true
//        // 缓存满了      含有 过期的 纹理  可以进行替换  true
//        // 缓存满了，并且纹理都是可用的  false
//        assert(!IsFull() || Contains_stale());
//
//        if (!IsFull()) {
//            Append(ch, glyphIndex);
//        } else {
//            // Find the first stale one, replace it
//            bool found = false;
//            hb_codepoint_t firstStaleIndex;
//            for (auto& kv : texture_cache_) {
//                if (!kv.second.fresh) {
//                    firstStaleIndex = kv.first;
//                    found = true;
//                }
//            }
//            assert(found);
//            Replace(ch, firstStaleIndex, glyphIndex);
//        }
//    }
//
//    bool TextureAtlas::IsFull() const {
//        return (texture_cache_.size() == static_cast<size_t>(kTextureDepth));
//    }
//
//    // 是否包含 无效纹理
//    bool TextureAtlas::Contains_stale() const {
//        for (auto& kv : texture_cache_) {
//            if (!kv.second.fresh) return true;
//        }
//        return false;
//    }
//
//    // 使缓存中所有的纹理都失效
//    void TextureAtlas::Invalidate() {
//        for (auto& kv : texture_cache_) {
//            kv.second.fresh = false;
//        }
//    }

    GLuint TextureAtlas::GetTexture() const { return texture_; }

}  // namespace texture_atlas
