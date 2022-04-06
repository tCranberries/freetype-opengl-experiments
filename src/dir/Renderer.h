//
// Created by wzw on 2022/3/25.
//

#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#include "Character.h"
#include "CharacterCache.h"
#include "FaceCollection.h"
#include "TextBox.h"
#include "Text.h"
#include "../texture_atlas.h"

class Renderer {
public:


    /**
     * 给出该文本框中各个文字的具体信息 Character  非自适应
     *
     * @param text              文本
     * @param faceCollection    font face 的集合
     * @param characterCache    字符缓存
     * @param textBox           文本框
     * @return                  各个 character
     */
    void getPosition(
            std::vector<std::vector<Character>>& res,
            Text& text,
            FaceCollection& faceCollection,
            CharacterCache& characterCache,
            TextBox& textBox,
            const std::vector<texture_atlas::TextureAtlas *> &texture_atlases);

    /**
     * 文本框的自适应版本
     *
     * @param text
     * @param faceCollection
     * @param characterCache
     * @param textBox
     * @param texture_atlases
     * @return
     */
    void getPositionAdapt(
            std::vector<std::vector<Character>>& res,
            Text& text,
            FaceCollection& faceCollection,
            CharacterCache& characterCache,
            TextBox& textBox,
            const std::vector<texture_atlas::TextureAtlas *> &texture_atlases);


    float getTextBoxAdaptRatio(Text& text,
                               FaceCollection& faceCollection,
                               TextBox& textBox,
                               const std::vector<texture_atlas::TextureAtlas *> &texture_atlases);


    /*
     * TODO: 可以当作数据成员存储一下
     *  计算距离的过程中，考虑空格的影响，空格不当做字符计算距离
     *  字符与字符之间的 相对位置是不会发生变化的， 不会影响到自适应等操作
     */
    /**
     * 更改字符集中的坐标达到 左对齐，水平居中，右对齐
     *
     * @param characters    字符集
     * @param textBox       文本框
     * @param alignment     对齐方式
     */
    void horizontalCentering(std::vector<std::vector<Character>>& characters, const TextBox& textBox, Alignment alignment);

    /**
     * 水平方向的上下居中，先处理中间行，然后依次处理上边的行 和 下边的行
     *
     * @param characters
     * @param textBox
     */
    void verticalCentering(std::vector<std::vector<Character>>& characters, const TextBox& textBox, unsigned int lineHeight);


private:

    std::vector<LineProperty> getLineProperties(std::vector<std::vector<Character>>& characters);

    void renderLine(
            std::vector<Character>& characters,
            Text& text,
            const std::string& line,
            FaceCollection& faceCollection,
            CharacterCache& characterCache,
            const std::vector<texture_atlas::TextureAtlas *> &texture_atlases,
            bool textureFlag);

};


#endif //OPENGL_RENDERER_H
