//
// Created by wzw on 2022/3/21.
//

#include "TextBox.h"

TextBox::TextBox(unsigned int x, unsigned int y, unsigned int width, unsigned int height, int topOff, int bottomOff, bool ad) {
    lbPosition.x = x;
    lbPosition.y = y;
    boxWidth = width;
    boxHeight = height;
    topOffset = topOff;
    bottomOffset = bottomOff;
    isAdapt = ad;
}




FT_Vector TextBox::getLeftBottomPosition() const {
    return lbPosition;
}

FT_Vector TextBox::getLeftTopPosition() {
    ltPosition.y = (int)((float)lbPosition.y + (float)boxHeight * ratio);
    ltPosition.x = lbPosition.x;

    return ltPosition;
}


unsigned int TextBox::getBoxWidth() const {
    return (int)((float)boxWidth * ratio);
}

unsigned int TextBox::getBoxHeight() const {
    return (int)((float)boxHeight * ratio);
}

unsigned int TextBox::getIsAdapt() const {
    return isAdapt;
}

int TextBox::getTopOffset() const {
    return topOffset;
}

void TextBox::setTopOffset(int topOff) {
    TextBox::topOffset = topOff;
}

int TextBox::getBottomOffset() const {
    return bottomOffset;
}

void TextBox::setBottomOffset(int bottomOff) {
    TextBox::bottomOffset = bottomOff;
}
