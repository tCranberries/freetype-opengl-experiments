//
// Created by wzw on 2022/3/21.
//

#include <cassert>
#include "Text.h"
#include "Util.h"

Text::Text(const char* filePath, TextDirection direction, TextLanguage language, unsigned int width, unsigned int height, bool spaceDecide,
           bool bold, bool italic, double an, double _lineScale, int _kerning, bool vertical) {
    readFile(filePath);
    textDirection = direction;
    textLanguage = language;
    fontPixelWidth = width;
    fontPixelHeight = height;
    isBold = bold;
    isItalic = italic;
    gradient = an;
    lineScale = _lineScale;
    kerning = _kerning;
    isSpaceDecideNextLine = spaceDecide;
    isVertical = vertical;

    lineHeight = (int)((float)fontPixelHeight * lineScale);
    columnWidth = (int)((float)fontPixelWidth * lineScale);
}

Text::~Text() {
    lines.clear();
    shapeCache.clear();
}

bool Text::getIsSpaceDecidedNextLine() const {
    return isSpaceDecideNextLine;
}

TextDirection Text::getTextDirection() const {
    return textDirection;
}

TextLanguage Text::getTextLanguage() const {
    return textLanguage;
}

unsigned int Text::getFontPixelWidth() const {
    return fontPixelWidth;
}

unsigned int Text::getFontPixelHeight() const {
    return fontPixelHeight;
}

unsigned int Text::getLineHeight() {
    lineHeight = (int)((float)fontPixelHeight * lineScale);
    return lineHeight;
}

unsigned int Text::getColumnWidth() {
    columnWidth = (int)((float)fontPixelWidth * lineScale);
    return columnWidth;
}

bool Text::getIsBold() const {
    return isBold;
}

bool Text::getIsItalic() const {
    return isItalic;
}

double Text::getAngle() const {
    return gradient;
}

std::vector<std::string>& Text::getLines() {
    return lines;
}


ErrorCode Text::getErrorCode() const {
    return errorCode;
}

bool Text::isShapeCacheContainsLine(const std::string& line) {
    auto it = shapeCache.find(line);
    if (it == shapeCache.end()) {
        return false;
    }
    return true;
}

const std::vector<ShapeCacheElement>& Text::getShapeCache(const std::string& line) {
    return shapeCache[line];
}

void Text::insertShapeCache(const std::string& line, std::vector<ShapeCacheElement> tmp) {
    shapeCache[line] = std::move(tmp);
}


void Text::readFile(const char* filePath) {
    if (filePath == nullptr) {
        errorCode = ErrorCode::text_error;
    }
    assert(filePath != nullptr);

    std::ifstream file(filePath);
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    /**
     * TODO: 去除渲染文本前后多余的换行空格等
     */
    Util::trimLines(lines);
    assert(!lines.empty());
}

double Text::getLineScale() const {
    return lineScale;
}

int Text::getKerning() const {
    return kerning;
}

void Text::setLineScale(double _lineScale) {
    lineScale = _lineScale;
}

void Text::setKerning(int _kerning) {
    kerning = _kerning;
}

void Text::setAngle(double _angle) {
    gradient = _angle;
}
