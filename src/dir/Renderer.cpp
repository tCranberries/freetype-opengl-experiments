//
// Created by wzw on 2022/3/25.
//

#include "Renderer.h"
#include "Util.h"


void Renderer::renderLine(
        std::vector<Character>& characters,
        Text& text,
        const std::string& line,
        FaceCollection& faceCollection,
        CharacterCache& characterCache,
        const std::vector<texture_atlas::TextureAtlas *> &texture_atlases,
        bool textureFlag) {

        /*
         * 该行字符的 face glyph index
         */
        std::vector<ShapeCacheElement> lineGlyphCache;
        if (text.isShapeCacheContainsLine(line)) {
            lineGlyphCache = text.getShapeCache(line);
        }

        for (auto& glyphCache: lineGlyphCache) {
            hb_codepoint_t curGlyphIndex = glyphCache.glyphIndex;
            unsigned int curFaceIndex = glyphCache.faceIndex;
            bool isEmoji = glyphCache.isEmoji;
            unsigned int codepoint = glyphCache.codepoint;

            FT_Face curFace;
            if (isEmoji) {
                curFace = faceCollection.getEmojiFaces()[curFaceIndex].getFace();
            } else {
                curFace = faceCollection.getFaces()[curFaceIndex].getFace();
            }

            /*
             * 看 character cache 中是否存在 glyph
             */
            Character *tmp = characterCache.getCharacter(curGlyphIndex);
            if (tmp != nullptr) {
                characters.push_back(*tmp);
                continue;
            }
            Character character;
            if (!characterCache.isFull() || characterCache.containsStale()) {
                faceCollection.renderGlyph(text, character, curFace, curGlyphIndex);
                if (textureFlag) {
                    if (isEmoji) {
                        texture_atlases[1]->Insert(character);
                    } else {
                        texture_atlases[0]->Insert(character);
                    }
                }

                character.setEmoji(isEmoji);
                character.setCodepoint(codepoint);
                characterCache.insert(curGlyphIndex, character);

                characters.push_back(character);
            }
        }
}


void Renderer::getPosition(
        std::vector<std::vector<Character>>& res, Text &text, FaceCollection &faceCollection,
        CharacterCache &characterCache, TextBox &textBox,
        const std::vector<texture_atlas::TextureAtlas *> &texture_atlases) {

    std::vector<std::string> lines = text.getLines();
    assert(!lines.empty());

    /**
     * 字间距 行间距
     */
    int kerning = text.getKerning();
    unsigned int lineHeight = text.getLineHeight();

    /**
     * 文本框左上角坐标
     */
    unsigned int textBoxTopX = textBox.getLeftTopPosition().x;
    unsigned int textBoxTopY = textBox.getLeftTopPosition().y;
    unsigned int textBoxBottomY =  textBox.getLeftBottomPosition().y;
    unsigned int textBoxWidth = textBox.getBoxWidth();
    int textBoxTopOffset = textBox.getTopOffset();
    int textBoxBottomOffset = textBox.getBottomOffset();


    int x = (int)textBoxTopX;
    int y = (int)textBoxTopY - textBoxTopOffset;
    int minYPosition = INT32_MAX;
    for (auto& line : lines) {
        std::vector<Character> characters;
        renderLine(characters, text, line, faceCollection, characterCache, texture_atlases, true);

        /**
         * position
         */
        unsigned int lastIndex {};
        // 存放换行后每一行的 字符
        std::vector<Character> eachLine;
        for (unsigned int i = 0; i < characters.size(); i++) {
            Character& character = characters[i];
            // 该字符的 宽高
            int w, h;
            // 该字符画笔的坐标 origin
            int xPos, yPos;
            int advance;

            if (character.getEmoji()) {
                auto ratioX = (float)text.getFontPixelWidth() / (float)character.getSize().x;
                auto ratioY = (float)text.getFontPixelHeight() / (float)character.getSize().y;

                w = (int)((float)character.getSize().x * ratioX);
                h = (int)((float)character.getSize().y * ratioY);

                xPos = x + (int)((float)character.getBearing().x * ratioX);
                yPos = y - (int)((float)(character.getSize().y - character.getBearing().y) * ratioY);
                advance = w;
            }
            else {
                w = character.getSize().x;
                h = character.getSize().y;
                xPos = x + character.getBearing().x;
                yPos = y - (character.getSize().y - character.getBearing().y);
                advance = (int)(character.getAdvance() >> 6);
            }
            character.setW(w);
            character.setH(h);
            character.setPosition(xPos, yPos);
            minYPosition = yPos < minYPosition ? yPos : minYPosition;

            eachLine.push_back(character);

            // TODO: 字间距此处设置， 增加 kerning
            x += advance + kerning;

            /**
             * TODO: 规定需要换行语言的范围， 哪些语言按照空格换行，哪些语言单独换行
             * 更改坐标，处理换行操作, 按照空格进行换行，根据 codepoint 判断空格
             */
            {
                bool changeLine = false;
                if (!text.getIsSpaceDecidedNextLine() || (Util::isSpaceCharacter(character.getCodepoint()) || i == characters.size() - 1)) {
                    if (x < (int)(textBoxTopX + textBoxWidth)) {
                        lastIndex = i;
                        continue;
                    }
                    eachLine.erase(eachLine.end() - (i - lastIndex), eachLine.end());
                    // 回退到上一个空格的位置，然后进行换行
                    i = lastIndex;
                    changeLine = true;
                }
                if (changeLine) {
                    // 该行内的换行操作
                    if (minYPosition < (int)(textBoxBottomY + textBoxBottomOffset)) {
                        break;
                    }
                    res.push_back(eachLine);
                    eachLine.clear();

                    x = (int)textBoxTopX;
                    y -= (int)lineHeight;
                }
            }
        }

        x = (int)textBoxTopX;
        y -= (int)lineHeight;

        // 最后一行一定在此处 push

        if (minYPosition < (int)(textBoxBottomY + textBoxBottomOffset)) {
            break;
        }
        res.push_back(eachLine);
    }
}

/*
 * 文本框自适应，等比缩放
 */
void Renderer::getPositionAdapt(std::vector<std::vector<Character>>& res, Text &text, FaceCollection &faceCollection,
                                                               CharacterCache &characterCache, TextBox &textBox,
                                                               const std::vector<texture_atlas::TextureAtlas *> &texture_atlases) {

    float ratio = getTextBoxAdaptRatio(text, faceCollection, textBox, texture_atlases);
    std::cout << "ratio: " << ratio << std::endl;
    textBox.setRatio(ratio);

    getPosition(res, text, faceCollection, characterCache, textBox, texture_atlases);
}



float Renderer::getTextBoxAdaptRatio(Text& text, FaceCollection& faceCollection, TextBox& textBox,
                                     const std::vector<texture_atlas::TextureAtlas *> &texture_atlases) {

    std::vector<std::string> lines = text.getLines();
    assert(!lines.empty());

    /*
     * 使用一个单独的  character cache
     */
    CharacterCache characterCache;

    /*
     * 字间距 行间距
     */
    int kerning = text.getKerning();
    unsigned int lineHeight = text.getLineHeight();

    /**
     * 文本框左上角坐标
     */
    unsigned int textBoxX = textBox.getLeftTopPosition().x;
    unsigned int textBoxBottomY = textBox.getLeftBottomPosition().y;   // 底线
    int textBoxTopOffset = textBox.getTopOffset();
    int textBoxBottomOffset = textBox.getBottomOffset();

    /**
     * 在此处使用 二分法 计算文本框合适的缩放比   条件  min(yPos) >= leftBottomTextBoxY
     * 初始 右边界 为 10 倍， 后续得计算
     */
    // TODO: 首先判断是否需要自适应，只有在放不下的时候采用自适应
    bool needAdapt = false;

    float leftBound = 1.0;
    float rightBound = 10.0;
    float lastRatio = 1.0f;
    while (leftBound <= rightBound) {
        auto minYPosition = INT32_MAX;
        float middle = needAdapt ? leftBound + (rightBound - leftBound) / 2 : 1.0f;

        unsigned int textBoxWidth = (int)((float)textBox.getBoxWidth() * middle);
        unsigned int textBoxHeight = (int)((float)textBox.getBoxHeight() * middle);
        int x = (int)textBoxX;
        int y = (int)(textBoxBottomY + textBoxHeight - textBoxTopOffset);

        for (auto& line : lines) {
            std::vector<Character> characters;
            renderLine(characters, text, line, faceCollection, characterCache, texture_atlases, false);

            /**
             * position
             */
            unsigned int lastSpaceIndex {};
            for (unsigned int i = 0; i < characters.size(); i++) {
                Character& character = characters[i];

                // 该字符的 宽高
                int w, h;
                // 该字符画笔的坐标 origin
                int xPos, yPos;
                int advance;

                if (character.getEmoji()) {
                    auto ratioX = (float)text.getFontPixelWidth() / (float)character.getSize().x;
                    auto ratioY = (float)text.getFontPixelHeight() / (float)character.getSize().y;

                    w = (int)((float)character.getSize().x * ratioX);
                    h = (int)((float)character.getSize().y * ratioY);

                    xPos = x + (int)((float)character.getBearing().x * ratioX);
                    yPos = y - (int)((float)(character.getSize().y - character.getBearing().y) * ratioY);
                    advance = w;
                }
                else {
                    w = character.getSize().x;
                    h = character.getSize().y;
                    xPos = x + character.getBearing().x;
                    yPos = y - (character.getSize().y - character.getBearing().y);
                    advance = (int)(character.getAdvance() >> 6);
                }
                character.setW(w);
                character.setH(h);
                character.setPosition(xPos, yPos);

                minYPosition = yPos < minYPosition ? yPos : minYPosition;

                x += advance + kerning;

                /**
                 * 更改坐标，处理换行操作, 按照空格进行换行，根据 codepoint 判断空格
                 */
                {
                    bool changeLine = false;
                    if (!text.getIsSpaceDecidedNextLine() || (Util::isSpaceCharacter(character.getCodepoint()) || i == characters.size() - 1)) {
                        if (x  < (int)textBoxX + (int)textBoxWidth) {
                            lastSpaceIndex = i;
                            continue;
                        }
                        // 回退到上一个空格的位置，然后进行换行
                        i = lastSpaceIndex;
                        changeLine = true;
                    }
                    if (changeLine) {
                        std::cout << "minYPos: " << minYPosition << std::endl;
                        x = (int)textBoxX;
                        y -= (int)lineHeight;
                    }
                }

            } // for

            // 该行基线原点的 坐标 x y
            x = (int)textBoxX;
            y -= (int)lineHeight;

            std::cout << "minYPos: " << minYPosition << std::endl;
        }

        if (minYPosition < (int)textBoxBottomY + textBoxBottomOffset) {
            if (!needAdapt) {
                needAdapt = true;
            }
            leftBound = middle + 0.001f;
        }
        else if (minYPosition >= (int)textBoxBottomY + textBoxBottomOffset) {
            lastRatio = middle;
            if (!needAdapt) {
                break;
            }
            rightBound = middle - 0.001f;
        }
    }

    return lastRatio;
}




void Renderer::horizontalCentering(std::vector<std::vector<Character>>& characters, const TextBox& textBox, Alignment alignment) {
    std::vector<LineProperty> lineProperties = getLineProperties(characters);

    for (unsigned int i = 0; i < lineProperties.size(); i++) {
        LineProperty lineProperty = lineProperties[i];

        int xOffset{};
        switch (alignment) {
            case Alignment::LEFT_ALIGNMENT:
                xOffset = -lineProperty.leftBound;
                break;
            case Alignment::CENTERING:
                xOffset = (int)((textBox.getBoxWidth() - (lineProperty.rightBound - lineProperty.leftBound)) / 2.0);
                break;
            case Alignment::RIGHT_ALIGNMENT:
                xOffset = (int)textBox.getBoxWidth() - lineProperty.rightBound;
                break;
            default:
                break;
        }

        for (auto& j : characters[i]) {
            int newXPos = j.getPosition().x + xOffset;
            j.setPosition(newXPos, j.getPosition().y);
        }
    }
}

void Renderer::verticalCentering(std::vector<std::vector<Character>>& characters, const TextBox& textBox, unsigned int lineHeight) {
    std::vector<LineProperty> lineProperties = getLineProperties(characters);
    assert(!lineProperties.empty());

    int yOffset = -(int)(((lineProperties[0].baseLine + lineHeight + lineProperties[lineProperties.size() - 1].bottomBound) - textBox.getBoxHeight()) / 2.0);

    for (auto& vec : characters) {
        for (auto& ele : vec) {
            ele.setPosition(ele.getPosition().x, ele.getPosition().y + yOffset);
        }
    }
}


std::vector<LineProperty> Renderer::getLineProperties(std::vector<std::vector<Character>>& characters) {
    assert(!characters.empty());
    std::vector<LineProperty> lineProperties;

    for (auto& vec : characters) {
        LineProperty lineProperty;
        int baseLine{};
        int leftBound{};
        int rightBound{};
        int topBound = INT32_MIN;
        int bottomBound = INT32_MAX;

        unsigned int first = 0;
        // 记录最后一个不是空格的字符
        unsigned int last = vec.size() - 1;
        // 寻找第一个不是空格的字符
        while (first < vec.size() && Util::isSpaceCharacter(vec[first].getCodepoint())) {
            first++;
        }
        while (last && Util::isSpaceCharacter(vec[last].getCodepoint())) {
            last--;
        }

        for (unsigned int i = first; i <= last; i++) {
            if (baseLine < vec[i].getPosition().y) {
                baseLine = vec[i].getPosition().y;
            }

            if (topBound < vec[i].getPosition().y + vec[i].getSize().y) {
                topBound = vec[i].getPosition().y + vec[i].getSize().y;
            }

            if (bottomBound > vec[i].getPosition().y) {
                bottomBound = vec[i].getPosition().y;
            }

            if (i == first) {
                leftBound = vec[i].getPosition().x;
            }

            if (i == last) {
                if (Util::isEmojiCharacter(vec[i].getCodepoint())) {
                    rightBound = vec[i].getPosition().x + vec[i].getW();
                }
                else {
                    rightBound = vec[i].getPosition().x + (int)(vec[i].getAdvance() >> 6);
                }
            }
        }

        lineProperty.baseLine = baseLine;
        lineProperty.topBound = topBound;
        lineProperty.bottomBound = bottomBound;
        lineProperty.leftBound = leftBound;
        lineProperty.rightBound = rightBound;

        lineProperties.push_back(lineProperty);
    }

    return lineProperties;
}








