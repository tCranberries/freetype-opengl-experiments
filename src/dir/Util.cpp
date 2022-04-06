//
// Created by wzw on 2022/3/22.
//

#include "Util.h"

const char* Util::TextLanguageToString(TextLanguage language) {
    static std::map<TextLanguage, const char*> m = {
            {TextLanguage::en, "en"},
            {TextLanguage::ar, "ar"},
            {TextLanguage::ch, "ch"}
    };
    return m[language];
}


bool Util::isEmojiCharacter(unsigned int codepoint) {
    return (codepoint >= 0x2600 && codepoint <= 0x27BF)        // 杂项符号与符号字体
           || codepoint == 0x303D
           || codepoint == 0x2049
           || codepoint == 0x203C
           || (codepoint >= 0x2000 && codepoint <= 0x200F)      //
           || (codepoint >= 0x2028 && codepoint <= 0x202F)      //
           || codepoint == 0x205F
           || (codepoint >= 0x2065 && codepoint <= 0x206F)      //
           /* 标点符号占用区域 */
           || (codepoint >= 0x2100 && codepoint <= 0x214F)      // 字母符号
           || (codepoint >= 0x2300 && codepoint <= 0x23FF)      // 各种技术符号
           || (codepoint >= 0x2B00 && codepoint <= 0x2BFF)      // 箭头A
           || (codepoint >= 0x2900 && codepoint <= 0x297F)      // 箭头B
           || (codepoint >= 0x3200 && codepoint <= 0x32FF)      // 中文符号
           || (codepoint >= 0xD800 && codepoint <= 0xDFFF)      // 高低位替代符保留区域
           || (codepoint >= 0xE000 && codepoint <= 0xF8FF)      // 私有保留区域
           || (codepoint >= 0xFE00 && codepoint <= 0xFE0F)      // 变异选择器
           || codepoint >= 0x10000;                             // Plane在第二平面以上的，char都不可以存，全部都转
}


bool Util::isSpaceCharacter(unsigned int codepoint) {
    return codepoint == 0x0020 || codepoint == 0x3000;
}


/**
 * 0: common left to right
 * 1: arabic
 * 2: hebrew
 * 3: ...
 *
 */
unsigned int Util::classify(unsigned int codepoint) {
//    if ((codepoint >= 0 && codepoint <= 0x036F)) {  // latin
//        return 0;
//    }
//
//    if (codepoint >= 0x2E80 && codepoint <= 0x9FFF) { // ch japan kar
//        return 0;
//    }

    if ((codepoint >= 0x0600 && codepoint <= 0x06FF) || (codepoint >= 0x0750 && codepoint <= 0x077F)) {
        return 1;
    }

    return 0;
}


void Util::trimLines(std::vector<std::string>& lines) {
    unsigned int i = 0;
    while (i < lines.size() && isEmptyLine(lines[i])) {
        i++;
    }
    lines.erase(lines.begin(), lines.begin() + i);

    int j = (int)lines.size() - 1;
    while (j >= 0 && isEmptyLine(lines[j])) {
        lines.erase(lines.begin() + j);
        j--;
    }
}


bool Util::isEmptyLine(std::string& string) {
    bool res{};
    if (string.empty()) {
        res = true;
    }
    for (auto ele : string) {
        if (ele != ' ') {
            res = false;
            break;
        }
    }
    return res;
}












