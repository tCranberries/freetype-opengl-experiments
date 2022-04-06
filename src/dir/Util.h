//
// Created by wzw on 2022/3/22.
//

#ifndef OPENGL_UTIL_H
#define OPENGL_UTIL_H

#include <map>
#include <vector>
#include "TextLanguage.h"

class Util {
public:
    static const char* TextLanguageToString(TextLanguage language);

    /**
     * 判断是否是 emoji
     *
     * @param codepoint    codepoint
     * @return             bool
     */
    static bool isEmojiCharacter(unsigned int codepoint);

    static bool isSpaceCharacter(unsigned int codepoint);

    static bool isArabic(unsigned int codepoint);






    static void trimLines(std::vector<std::string>& lines);

private:
    /**
     * 是否是空行，无值或者全都是空格视为空行
     *
     * @param string    string
     * @return          true
     */
    static bool isEmptyLine(std::string& string);
};


#endif //OPENGL_UTIL_H
