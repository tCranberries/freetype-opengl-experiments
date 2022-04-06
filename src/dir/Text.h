//
// Created by wzw on 2022/3/21.
//

#ifndef OPENGL_TEXT_H
#define OPENGL_TEXT_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <algorithm>

#include "TextDirection.h"
#include "TextLanguage.h"
#include "ErrorCode.h"

/*
 * 目前一个 text 对象只保存一种语言的相关信息，不能多语言夹杂在一起 （因为无法判断当前语言，方向）
 *
 * 使用 笛卡尔 坐标系
 *  ^  y
 *  |
    |
    |__________> x
 (0, 0)
 */

typedef struct {
    bool isEmoji;
    unsigned int codepoint;
    unsigned int faceIndex;
    unsigned int glyphIndex;
} ShapeCacheElement;


/**
 * 定义一行的属性，基线的位置，最左侧可见文字的位置，最右侧可见文字的位置等
 */
struct LineProperty {
    int baseLine{};
    int leftBound{};
    int rightBound{};

    // 该行上边界位置    baseLine + text.getLineHeight
    int upperBound{};

    // 一行中 字符的 上下最大最下 位置
    int topBound{};
    int bottomBound{};

    int yOffset{};              // 为达到居中，y 轴的偏移
    int xOffset{};              // 为达到居中，x 轴的偏移
};

enum Alignment {
    LEFT_ALIGNMENT,
    CENTERING,
    RIGHT_ALIGNMENT
};

/*
 * TODO: 后续需要提取  LanguageProperty struct 封装相关属性
 */


class Text {
private:
    // text
    std::vector<std::string> lines;

    TextDirection textDirection = TextDirection::LTR;

    TextLanguage textLanguage = TextLanguage::en;

    unsigned int fontPixelWidth{};

    unsigned int fontPixelHeight{};

    // 行高  水平分布
    unsigned int lineHeight{};
    // 列宽  垂直分布
    unsigned int columnWidth{};
    // 行高基于字号的倍数， 默认 1， 用于调整行高和列宽
    double lineScale = 1.35;

    /*
     * 字体间距
     * 负数： 缩小字体间距
     * 正数： 增大字体间距
     * 0  ： 不变
     */
    int kerning = 0;


    // TODO: 是否加粗， 后续可以设置等级， 加粗等级和变细等级
    bool isBold{};

    // 是否斜体
    bool isItalic{};
    // 当设置斜体的时候，使用该字体倾斜度  0 ~ 1， 默认 0
    double gradient{};


    // 是否是 纵向 排版
    bool isVertical{};


    /*
     * TODO: 是否按空格进行换行
     * 英语，法语，阿拉伯语等需要按照 空格进行换行，也就是按词换行
     * 汉字日文等 不需要按词换行
     */
    bool isSpaceDecideNextLine{};

    // 针对每一行 text 的缓存，存放一行中 每个字符 对应的 mFace index，glyph index
    std::unordered_map<std::string, std::vector<ShapeCacheElement>> shapeCache;

    ErrorCode errorCode = ErrorCode::ok;

public:
    Text() = default;

    Text(const char* filePath,
         TextDirection direction,
         TextLanguage language,
         unsigned int width,
         unsigned int height,
         bool spaceDecide,
         bool bold = false,
         bool italic = false,
         double an = 0,
         double _lineScale = 1.35,
         int _kerning = 0,
         bool vertical = false);

    ~Text();

    bool getIsSpaceDecidedNextLine() const;

    // getter and setter
    TextDirection getTextDirection() const;

    TextLanguage getTextLanguage() const;

    unsigned int getFontPixelWidth() const;

    unsigned int getFontPixelHeight() const;

    unsigned int getLineHeight();

    unsigned int getColumnWidth();

    bool getIsBold() const;

    bool getIsItalic() const;

    double getAngle() const;

    double getLineScale() const;

    int getKerning() const;

    void setLineScale(double _lineScale);

    void setKerning(int _kerning);

    void setAngle(double _angle);

    bool getIsVertical() const { return isVertical; }

    std::vector<std::string>& getLines();

    ErrorCode getErrorCode() const;

    // cache
    bool isShapeCacheContainsLine(const std::string& line);

    const std::vector<ShapeCacheElement>& getShapeCache(const std::string& line);

    void insertShapeCache(const std::string& line, std::vector<ShapeCacheElement> tmp);


private:
    void readFile(const char* filePath);

};


#endif //OPENGL_TEXT_H



















