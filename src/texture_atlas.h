// Copyright 2019 <Andrea Cognolato>
// TODO(andrea): use a better map, try in some way to have dynamic gpu memory
// allocation, find a better data structure
#ifndef SRC_TEXTURE_ATLAS_H_
#define SRC_TEXTURE_ATLAS_H_

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_LCD_FILTER_H

#include <harfbuzz/hb.h>

#include <glad/glad.h>

#include <unordered_map>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

#include "dir/Character.h"


namespace texture_atlas {
    using std::pair;
    using std::unordered_map;
    using std::vector;

//    struct Character {
//                                        // 在插入到 纹理缓存 中时获取相关信息
//        size_t texture_array_index;     // 在纹理缓存中的位置  index
//        glm::vec2 texture_coordinates;  // 纹理的 坐标
//        size_t texture_id;              // 纹理的 id
//
//        glm::ivec2 size;                // glyph 相关， 在 RenderGlyph 中通过  glyph 的相关信息获取
//        glm::ivec2 bearing;
//        GLuint advance;
//        bool colored;
//    };

    class TextureAtlas {
    private:
//        typedef struct {
//            Character character;
//            bool fresh;
//        } cache_element_t;

        // 当前纹理位置
        GLuint index_ = 0;
        GLuint texture_ = 0;
        GLsizei textureWidth_, textureHeight_;

        /*
         * glyph index -> Character， 缓存大小 1024
         */
        //unordered_map<hb_codepoint_t, cache_element_t> texture_cache_;
        GLenum format_;

    public:
        TextureAtlas(GLsizei textureWidth,
                     GLsizei textureHeight,
                     GLuint shaderProgramId,
                     const char* textureUniformLocation,
                     GLenum internalformat,
                     GLenum format,
                     GLint shader_texture_index);

        ~TextureAtlas();

        // void Insert(const vector<unsigned char>& bitmap_buffer, GLsizei width, GLsizei height, Character* ch, GLuint offset);
        void Insert(Character& ch);

//        void Insert(hb_codepoint_t glyphIndex, pair<Character, vector<unsigned char>>* ch);
//
//        void Append(pair<Character, vector<unsigned char>>* p, hb_codepoint_t codepoint);
//
//        void Replace(pair<Character, vector<unsigned char>>* p, hb_codepoint_t stale, hb_codepoint_t codepoint);
//
//        bool Contains(hb_codepoint_t codepoint) const;
//
//        Character* Get(hb_codepoint_t codepoint);
//
//        bool IsFull() const;
//
//        bool Contains_stale() const;
//
//        void Invalidate();

        GLuint GetTexture() const;
    };
}  // namespace texture_atlas

#endif  // SRC_TEXTURE_ATLAS_H_
