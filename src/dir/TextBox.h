//
// Created by wzw on 2022/3/21.
//

#ifndef OPENGL_TEXTBOX_H
#define OPENGL_TEXTBOX_H

#include <ft2build.h>
#include FT_IMAGE_H

class TextBox {
private:

    // 左下角坐标
    FT_Vector lbPosition{};
    // 左上角坐标
    FT_Vector ltPosition{};

    unsigned int boxWidth{};

    unsigned int boxHeight{};

    bool isAdapt = true;

    // 缩放比
    float ratio = 1;

    /**
     * 上下边界偏移量   表示第一行文字或最后一行文字的基线距离文本框上下边界距离，可以超出，表示可超出文本框
     * + 内部
     * - 外部
     */
     int topOffset{};

     int bottomOffset{};

public:
    TextBox() = default;

    TextBox(unsigned int x, unsigned int y, unsigned int width, unsigned int height, int topOff, int bottomOff, bool ad = false);

    FT_Vector getLeftBottomPosition() const;

    FT_Vector getLeftTopPosition();

    unsigned int getBoxWidth() const;

    unsigned int getBoxHeight() const;

    unsigned int getIsAdapt() const;

    int getTopOffset() const;

    void setTopOffset(int topOffset);

    int getBottomOffset() const;

    void setBottomOffset(int bottomOffset);

    void setRatio(float r) { ratio = r; }

    float getRatio() const { return ratio; }
};


#endif //OPENGL_TEXTBOX_H











