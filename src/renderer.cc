// Copyright 2019 <Andrea Cognolato>
#include "./renderer.h"
#include "./constants.h"

namespace renderer {
    void Render(Renderer fontRender,
            const Shader &shader,
                Text& text,
                FaceCollection& faceCollection,
                const vector<TextureAtlas *> &texture_atlases,
                GLuint VAO,
                GLuint VBO) {

        // Set background color
        glClearColor(BACKGROUND_COLOR);
        glClear(GL_COLOR_BUFFER_BIT);

        CharacterCache characterCache;

        int topOffset = (int)(text.getFontPixelWidth() * 1.0);
        int bottomOffset = 10;

        TextBox textBox(0, 0, 1800, 1000, topOffset, bottomOffset, false);
        //Alignment alignment = Alignment::LEFT_ALIGNMENT;

        // 横向排版布局
        std::vector<std::vector<Character>> res;
        if (textBox.getIsAdapt()) {
            fontRender.getPositionAdapt(res, text, faceCollection, characterCache, textBox, texture_atlases);
        }
        else {
            fontRender.getPosition(res, text, faceCollection, characterCache, textBox, texture_atlases);
        }

        /*
         * TODO: 最终渲染坐标的计算， 针对文本框来处理 居中 对齐等
         */
        // 水平方向的  左对齐，右对齐，居中
//        {
//           fontRender.horizontalCentering(res, textBox, alignment);
//        }
//
//         // 水平方向的 上下居中
//        {
//            fontRender.verticalCentering(res, textBox, text.getLineHeight());
//        }


        for (const auto& vec : res) {
            std::cout << "===========================================" << std::endl;
            for (const auto& ele : vec) {
                std::cout << "ele pos: (" << ele.getPosition().x << ", " << ele.getPosition().y << "),   w: " << ele.getW() << "  h: " << ele.getH()  << "    advance: " << ele.getAdvance() << std::endl;
            }
        }





         for (unsigned int i = 0; i < res.size(); i++) {
            auto chars = res[i];

            glBindVertexArray(VAO);

            vector<array<array<GLfloat, 4>, 6>> quads;
            quads.resize(chars.size());
            vector<array<GLuint, 2>> texture_ids;
            texture_ids.resize(chars.size() * 6);

            for (unsigned int j = 0; j < chars.size(); j++) {
                Character character = chars[j];

                auto w = (float)character.getW();
                auto h = (float)character.getH();
                float xPos = (float)character.getPosition().x;
                float yPos = (float)character.getPosition().y;
                glm::vec2 tc = character.getTextureCoordinates();
                int texture_array_index = (int) character.getTextureArrayIndex();
                int colored = character.getColored();

                array<array<GLfloat, 4>, 6> quad = {{       // a
                                                            // |
                                                            // |
                                                            // |
                                                            // c--------b
                                                            {xPos, yPos, 0, tc.y},
                                                            {xPos, yPos + h, 0, 0},
                                                            {xPos + w, yPos, tc.x, tc.y},
                                                            // d--------f
                                                            // |
                                                            // |
                                                            // |
                                                            // e
                                                            {xPos, yPos + h, 0, 0},
                                                            {xPos + w, yPos, tc.x, tc.y},
                                                            {xPos + w, yPos + h, tc.x, 0}}};

                array<GLuint, 2> texture_id = {
                        static_cast<GLuint>(texture_array_index),
                        static_cast<GLuint>(colored)
                };

                quads[j] = quad;
                texture_ids[j * 6 + 0] = texture_ids[j * 6 + 1] =
                texture_ids[j * 6 + 2] = texture_ids[j * 6 + 3] =
                texture_ids[j * 6 + 4] = texture_ids[j * 6 + 5] = texture_id;

            }

            assert(6 * quads.size() == texture_ids.size());
            // Set the shader's uniforms
            glm::vec4 fg_color(FOREGROUND_COLOR);
            glUniform4fv(glGetUniformLocation(shader.programId, "fg_color_sRGB"), 1, glm::value_ptr(fg_color));

            // Bind the texture to the active texture unit
            for (size_t j = 0; j < texture_atlases.size(); j++) {
                glActiveTexture(GL_TEXTURE0 + j);
                glBindTexture(GL_TEXTURE_2D_ARRAY, texture_atlases[j]->GetTexture());
            }


            // 使用 glGenBuffers() 生成新缓存对象。
            // 使用 glBindBuffer() 绑定缓存对象。
            // 使用 glBufferData() 将顶点数据拷贝到缓存对象中。
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            {
                // Allocate memory
                GLsizeiptr total_size = quads.size() * (sizeof(quads[0]) + 6 * sizeof(texture_ids[0]));
                glBufferData(GL_ARRAY_BUFFER, total_size, nullptr, GL_STREAM_DRAW);

                // Load quads
                GLintptr offset = 0;
                GLsizeiptr quads_byte_size = quads.size() * (sizeof(quads[0]));
                glBufferSubData(GL_ARRAY_BUFFER, offset, quads_byte_size, quads.data());

                // Load texture_ids
                offset = quads_byte_size;
                GLsizeiptr texture_ids_byte_size = texture_ids.size() * (sizeof(texture_ids[0]));
                glBufferSubData(GL_ARRAY_BUFFER, offset, texture_ids_byte_size, texture_ids.data());

                // Tell shader that layout=0 is a vec4 starting at offset 0
                glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
                glEnableVertexAttribArray(0);

                // Tell shader that layout=1 is an ivec2 starting after
                // quads_byte_size
                glVertexAttribIPointer(1, 2, GL_UNSIGNED_INT, 2 * sizeof(GLuint),
                                       reinterpret_cast<const GLvoid *>(offset));
                glEnableVertexAttribArray(1);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // Render quads
            glDrawArrays(GL_TRIANGLES, 0, chars.size() * sizeof(chars[0]));

            glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

            characterCache.inValidate();
            glBindVertexArray(0);
        }
    }
}


//        // Create the shaping buffer
////        hb_buffer_t *buf = hb_buffer_create();
//
//        // Calculate how many lines to display
////        unsigned int start_line = state.GetStartLine();
////        unsigned int last_line;
////        if (state.GetVisibleLines() > text.getLines().size()) {
////            last_line = start_line + text.getLines().size();
////        } else {
////            last_line = start_line + state.GetVisibleLines();
////        }
//
//
//
////        // 给定文本框左上角起始坐标（0, 200） w: 200  h: 200
////        int textBoxX = 0;
////        int textBoxY = 200;
////        int textWidth = 200;
////        // int textHeight = 200;
////
////        auto x = textBoxX;
//
//        // For each visible line
//        for (unsigned int ix = start_line; ix < last_line; ix++) {
//            auto &line = text.getLines()[ix];
//
//            std::vector<ShapeCacheElement> codepoints_face_pair;
//            if (text.isShapeCacheContainsLine(line)) {
//                codepoints_face_pair = text.getShapeCache(line);
//            }
//
//            // 每一行对应的 起始  x  y 坐标
//            // 换行时产生的偏移量
//            int offsetY = 0;
//
//            // auto y = state.GetHeight() - (state.GetLineHeight() * ((ix - state.GetStartLine()) + 1));
//            auto y = textBoxY - (state.GetLineHeight() * (ix + offsetY - state.GetStartLine() + 1));
//            std::cout << "y: " << y << std::endl;
//
//            // Render the line to the screen, given the mFaces, the codepoints, the codepoint's textures and a texture atlas
//            {
//                glBindVertexArray(VAO);
//
//                // 每一行 glyph 的 num
//                size_t codepoints_in_line = codepoints_face_pair.size();
//                size_t i = 0;
//
//                // While I haven't rendered all codepoints
////                while (i < codepoints_in_line) {
//                    vector<Character> characters;
//
//                    for (; i < codepoints_in_line; ++i) {
//                        // glyph index
//                        hb_codepoint_t glyphIndex = codepoints_face_pair[i].glyphIndex;
//
//                        Character *ch = texture_atlases[0]->Get(glyphIndex);
//                        if (ch != nullptr) {
//                            characters.push_back(*ch);
//                            continue;
//                        }
//
//                        ch = texture_atlases[1]->Get(glyphIndex);
//                        if (ch != nullptr) {
//                            characters.push_back(*ch);
//                            continue;
//                        }
//
//                        // If I have got space in both
//                        if ((texture_atlases[0]->Contains_stale() || !texture_atlases[0]->IsFull()) &&
//                            (texture_atlases[1]->Contains_stale() || !texture_atlases[1]->IsFull())) {
////                            FT_Face face = get<0>(faces[codepoints_face_pair.first[i]]);
//                            FT_Face face;
//                            if (codepoints_face_pair[i].isEmoji) {
//                                face = faceCollection.getEmojiFaces().at(codepoints_face_pair[i].faceIndex).getFace();
//                            }
//                            else {
//                                face = faceCollection.getFaces().at(codepoints_face_pair[i].faceIndex).getFace();
//                            }
//
//                            // Get its texture's coordinates and offset from the atlas
//                            // pair<Character, vector<unsigned char>>   Character -> bite buffer
//                            // 此时的 character 已经有了相关属性
//                            auto p = RenderGlyph(face, glyphIndex);
//                            if (p.first.colored) {
//                                texture_atlases[1]->Insert(glyphIndex, &p);
//                            } else {
//                                texture_atlases[0]->Insert(glyphIndex, &p);
//                            }
//                            characters.push_back(p.first);
//                        }
//                        else {
//                            break;
//                        }
//                    }
//
//                    // TODO(andrea): instead of allocating on each line, allocate externally
//                    // and eventually resize here
//                    vector<array<array<GLfloat, 4>, 6>> quads;
//                    quads.resize(characters.size());
//                    vector<array<GLuint, 2>> texture_ids;
//                    texture_ids.resize(characters.size() * 6);
//
//
//
//                    /*
//                     * 计算每个 character 的 position ，填充 quads   texture_id 数组
//                     */
//                    for (size_t k = 0; k < characters.size(); ++k) {
//                        Character &ch = characters[k];
//                        // Calculate the character position
//                        GLfloat w, h;
//                        GLfloat xPos, yPos;
//                        // TODO(andrea): we should use harfbuzz's info
//                        GLuint advance;
//                        if (ch.colored) {
//                            // ratio  比例    16 / (int)mFace->glyph->bitmap.mWidth    17 / (int)mFace->glyph->bitmap.rows
//                            auto ratio_x = static_cast<GLfloat>(kFontPixelWidth) / static_cast<GLfloat>(ch.size.x);
//                            auto ratio_y = static_cast<GLfloat>(kFontPixelHeight) / static_cast<GLfloat>(ch.size.y);
//
//                            w = (float)ch.size.x * ratio_x;
//                            h = (float)ch.size.y * ratio_y;
//
//                            // x y 每一行文字的起始坐标，基线位置
//                            // y - (mFace->glyph->bitmap.rows - mFace->glyph->bitmap_top) * ratio_y
//                            xPos = (float)x + (float)ch.bearing.x * ratio_x;
//                            yPos = (float)y - (float)(ch.size.y - ch.bearing.y) * ratio_y;
//                            advance = (unsigned int)w;
//
//                        } else {
//                            w = (float)ch.size.x;
//                            h = (float)ch.size.y;
//
//                            xPos = (float)(x + ch.bearing.x);
//                            //
//                            yPos = (float)(y - (ch.size.y - ch.bearing.y));
//
//                            advance = (ch.advance >> 6);
//                        }
//                        std::cout << "x: " << x << std::endl;
//
//                        if (x > textBoxX + textWidth) {
//                            // 大于文本框宽度，进行换行
//                            std::cout << "outer" << std::endl;
//                            x = textBoxX;
//                            y -= state.GetLineHeight();
//                            offsetY++;
//                        }
//                        x += (int)advance;
//
//
//                        auto tc = ch.texture_coordinates;
//
//                        // FreeTypes uses a different coordinate convention,
//                        // so we need to render the quad flipped horizontally,
//                        // that's why where we should have 0 we have tc.y and vice versa
//                        // https://blog.csdn.net/thisway_diy/article/details/107981673
//                        array<array<GLfloat, 4>, 6> quad = {{       // a
//                                                                    // |
//                                                                    // |
//                                                                    // |
//                                                                    // c--------b
//                                                                    {xPos, yPos, 0, tc.y},
//                                                                    {xPos, yPos + h, 0, 0},
//                                                                    {xPos + w, yPos, tc.x, tc.y},
//                                                                    // d--------f
//                                                                    // |
//                                                                    // |
//                                                                    // |
//                                                                    // e
//                                                                    {xPos, yPos + h, 0, 0},
//                                                                    {xPos + w, yPos, tc.x, tc.y},
//                                                                    {xPos + w, yPos + h, tc.x, 0}}};
//
//                        array<GLuint, 2> texture_id = {
//                                static_cast<GLuint>(ch.texture_array_index),
//                                static_cast<GLuint>(ch.colored)
//                        };
//
//                        quads[k] = quad;
//                        texture_ids[k * 6 + 0] = texture_ids[k * 6 + 1] =
//                        texture_ids[k * 6 + 2] = texture_ids[k * 6 + 3] =
//                        texture_ids[k * 6 + 4] = texture_ids[k * 6 + 5] = texture_id;
//                    }
//
//
//
//
//                    assert(6 * quads.size() == texture_ids.size());
//                    // Set the shader's uniforms
//                    glm::vec4 fg_color(FOREGROUND_COLOR);
//                    glUniform4fv(glGetUniformLocation(shader.programId, "fg_color_sRGB"), 1, glm::value_ptr(fg_color));
//
//                    // Bind the texture to the active texture unit
//                    for (size_t j = 0; j < texture_atlases.size(); j++) {
//                        glActiveTexture(GL_TEXTURE0 + j);
//                        glBindTexture(GL_TEXTURE_2D_ARRAY, texture_atlases[j]->GetTexture());
//                    }
//
//
//                    // 使用 glGenBuffers() 生成新缓存对象。
//                    // 使用 glBindBuffer() 绑定缓存对象。
//                    // 使用 glBufferData() 将顶点数据拷贝到缓存对象中。
//                    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//                    {
//                        // Allocate memory
//                        GLsizeiptr total_size = quads.size() * (sizeof(quads[0]) + 6 * sizeof(texture_ids[0]));
//                        glBufferData(GL_ARRAY_BUFFER, total_size, nullptr, GL_STREAM_DRAW);
//
//                        // Load quads
//                        GLintptr offset = 0;
//                        GLsizeiptr quads_byte_size = quads.size() * (sizeof(quads[0]));
//                        glBufferSubData(GL_ARRAY_BUFFER, offset, quads_byte_size, quads.data());
//
//                        // Load texture_ids
//                        offset = quads_byte_size;
//                        GLsizeiptr texture_ids_byte_size = texture_ids.size() * (sizeof(texture_ids[0]));
//                        glBufferSubData(GL_ARRAY_BUFFER, offset, texture_ids_byte_size, texture_ids.data());
//
//                        // Tell shader that layout=0 is a vec4 starting at offset 0
//                        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
//                        glEnableVertexAttribArray(0);
//
//                        // Tell shader that layout=1 is an ivec2 starting after
//                        // quads_byte_size
//                        glVertexAttribIPointer(1, 2, GL_UNSIGNED_INT, 2 * sizeof(GLuint), reinterpret_cast<const GLvoid *>(offset));
//                        glEnableVertexAttribArray(1);
//                    }
//                    glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//                    // Render quads
//                    glDrawArrays(GL_TRIANGLES, 0, characters.size() * sizeof(characters[0]));
//
//                    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
//
////                    for (size_t j = 0; j < texture_atlases.size(); j++) {
////                        texture_atlases[j]->Invalidate();
////                    }
//                    characterCache.inValidate();
////                }
//
//                glBindVertexArray(0);
//            }
//        }
//
//        // Destroy the shaping buffer
////        hb_buffer_destroy(buf);
//    }

//    /**
//     * render glyph
//     *
//     * @param face        font mFace
//     * @param codepoint   glyph index
//     * @return            character --> bite buffer    渲染 glyph 为 bitmap buffer, 并通过 glyph 相关信息填充 character
//     */
//    pair<Character, vector<unsigned char>> RenderGlyph(FT_Face face, hb_codepoint_t glyphIndex) {
//        FT_Int32 flags = FT_LOAD_DEFAULT | FT_LOAD_TARGET_LCD;
//
//        if (FT_HAS_COLOR(face)) {
//            flags |= FT_LOAD_COLOR;
//        }
//
//        if (FT_Load_Glyph(face, glyphIndex, flags)) {
//            fprintf(stderr, "Could not load glyph with codepoint: %u\n", glyphIndex);
//            exit(EXIT_FAILURE);
//        }
//
//        if (!FT_HAS_COLOR(face)) {
//            if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_LCD)) {
//                fprintf(stderr, "Could not render glyph with codepoint: %u\n", glyphIndex);
//                exit(EXIT_FAILURE);
//            }
//        }
//
//        vector<unsigned char> bitmap_buffer;
//        if (!FT_HAS_COLOR(face)) {
//            // mFace->glyph->bitmap.buffer is a rows * pitch matrix,
//            // but we need a matrix which is rows * mWidth.
//            // For each row i, buffer[i][pitch] is just a padding byte, therefore we can ignore it
//            // RGB
//            bitmap_buffer.resize(face->glyph->bitmap.rows * face->glyph->bitmap.width * 3);
//            for (uint i = 0; i < face->glyph->bitmap.rows; i++) {
//                for (uint j = 0; j < face->glyph->bitmap.width; j++) {
//                    unsigned char ch = face->glyph->bitmap.buffer[i * face->glyph->bitmap.pitch + j];
//                    bitmap_buffer[i * face->glyph->bitmap.width + j] = ch;
//                }
//            }
//        } else {
//            // RGBA
//            bitmap_buffer.resize(face->glyph->bitmap.rows * face->glyph->bitmap.width * 4);
//            // result = last - first
//            copy(face->glyph->bitmap.buffer,
//                 face->glyph->bitmap.buffer + face->glyph->bitmap.rows * face->glyph->bitmap.width * 4,
//                 bitmap_buffer.begin());
//        }
//
//        GLsizei texture_width;
//        GLsizei texture_height;
//        if (FT_HAS_COLOR(face)) {
//            texture_width = (int)face->glyph->bitmap.width;
//            texture_height = (int)face->glyph->bitmap.rows;
//        } else {
//            // If the glyph is not colored then it is subpixel antialiased so the
//            // texture will have 3x the mWidth
//            texture_width = (int)face->glyph->bitmap.width / 3;
//            texture_height = (int)face->glyph->bitmap.rows;
//        }
//
//        Character ch{};
//        ch.size = glm::ivec2(texture_width, texture_height);
//        ch.bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
//        ch.advance = static_cast<GLuint>(face->glyph->advance.x);
//        ch.colored = static_cast<bool> FT_HAS_COLOR(face);
//
//        return make_pair(ch, bitmap_buffer);
//    }

//}  // namespace renderer
