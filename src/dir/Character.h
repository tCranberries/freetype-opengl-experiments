//
// Created by wzw on 2022/3/22.
//

#ifndef OPENGL_CHARACTER_H
#define OPENGL_CHARACTER_H

#include <glm/glm.hpp>

#include <vector>

class Character {
private:
    unsigned int textureArrayIndex{};

    unsigned int textureId{};

    glm::vec2 textureCoordinates{};

    // 当前笛卡尔坐标系下的 position
    glm::ivec2 position{};

    // bitmap->width
    // bitmap->rows
    glm::ivec2 size{};

    // bitmap_top
    // bitmap_left
    glm::ivec2 bearing{};

    unsigned int advance{};

    bool colored{};

    std::vector<unsigned char> bitmapBuffer;

    // 字体宽高
    int w{};

    int h{};

    bool isEmoji{};

    unsigned int codepoint{};

public:
    Character() = default;

    Character(unsigned int bitmapWidth,
              unsigned int bitmapRows,
              int bitmapLeft,
              int bitmapTop,
              unsigned int advanceX,
              bool isColorful);

    unsigned int calculateAdvance(unsigned int fontPixelWidth, unsigned int fontPixelHeight) const;


    std::vector<unsigned char>& getBitmapBuffer();

    void setSize(unsigned int bitmapWidth, unsigned bitmapRows);

    void setBearing(int bitmapLeft, int bitmapRight);

    void setAdvance(unsigned int advanceX);

    unsigned int getAdvance() const;

    void setColor(bool isColorful);

    bool getColored() const;

    void setTextureId(unsigned int texId);

    void setTextureArrayIndex(unsigned int textureIndex);

    void setTextureCoordinates(glm::vec2 coordinates);

    glm::vec2 getTextureCoordinates() const;

    unsigned int getTextureArrayIndex() const;

    glm::ivec2 getSize() const;

    glm::ivec2 getBearing() const;

    void setPosition(int xPos, int yPos);

    glm::ivec2 getPosition() const;

    int getW() const { return w; }

    int getH() const { return h; }

    void setW(int W) { w = W; }

    void setH(int H) { h = H; }

    void setEmoji(bool emoji) { isEmoji = emoji; }

    bool getEmoji() const { return isEmoji; }

    unsigned int getCodepoint() const { return codepoint; }

    void setCodepoint(unsigned int code) { codepoint = code; }
};


#endif //OPENGL_CHARACTER_H

























