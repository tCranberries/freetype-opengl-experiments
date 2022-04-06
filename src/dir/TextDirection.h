//
// Created by wzw on 2022/3/21.
//

#ifndef OPENGL_TEXTDIRECTION_H
#define OPENGL_TEXTDIRECTION_H

enum class TextDirection {
    INVALID = 0,
    LTR = 4,    // Text is set horizontally from left to right.
    RTL,        // Text is set horizontally from right to left.
    TTB,        // Text is set vertically from top to bottom.
    BTT         // Text is set vertically from bottom to top.
};

#endif //OPENGL_TEXTDIRECTION_H
