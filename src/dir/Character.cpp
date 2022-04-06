//
// Created by wzw on 2022/3/22.
//

#include "Character.h"


Character::Character(unsigned int bitmapWidth,
          unsigned int bitmapRows,
          int bitmapLeft,
          int bitmapTop,
          unsigned int advanceX,
          bool isColorful) {

    size = glm::ivec2(bitmapWidth, bitmapRows);
    bearing = glm::ivec2(bitmapLeft, bitmapTop);
    advance = advanceX;
    colored = isColorful;
}


unsigned int Character::calculateAdvance(unsigned int fontPixelWidth, unsigned int fontPixelHeight) const {
    if (isEmoji) {
        auto ratioX = (float)fontPixelWidth / (float)size.x;
        auto ratioY = (float)fontPixelHeight / (float)size.y;

        return (unsigned int)((float)size.x * ratioX);
    }
    return advance >> 6;
}


std::vector<unsigned char>& Character::getBitmapBuffer() {
    return bitmapBuffer;
}

void Character::setSize(unsigned int bitmapWidth, unsigned bitmapRows) {
    size = glm::ivec2(bitmapWidth, bitmapRows);
}

void Character::setBearing(int bitmapLeft, int bitmapTop) {
    bearing = glm::ivec2(bitmapLeft, bitmapTop);
}

void Character::setAdvance(unsigned int advanceX) {
    advance = advanceX;
}

unsigned int Character::getAdvance() const {
    return advance;
}

void Character::setColor(bool isColorful) {
    colored = isColorful;
}

bool Character::getColored() const {
    return colored;
}

void Character::setTextureId(unsigned int texId) {
    textureId = texId;
}


void Character::setTextureArrayIndex(unsigned int textureIndex) {
    textureArrayIndex = textureIndex;
}

void Character::setTextureCoordinates(glm::vec2 coordinates) {
    textureCoordinates = coordinates;
}

glm::vec2 Character::getTextureCoordinates() const {
    return textureCoordinates;
}

unsigned int Character::getTextureArrayIndex() const {
    return textureArrayIndex;
}

glm::ivec2 Character::getSize() const {
    return size;
}

glm::ivec2 Character::getBearing() const {
    return bearing;
}

void Character::setPosition(int xPos, int yPos) {
    position.x = xPos;
    position.y = yPos;
}

glm::ivec2 Character::getPosition() const {
    return position;
}




















