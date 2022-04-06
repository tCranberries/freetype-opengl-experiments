//
// Created by wzw on 2022/3/23.
//

#include "CharacterCache.h"
#include <algorithm>

// Minimal initial number of buckets
unsigned int CharacterCache::size = 1024;

CharacterCache::CharacterCache() : cache(size) {}

CharacterCache::~CharacterCache() {
    cache.clear();
}

/**
 * 在外部判断 是否可以 insert
 * insert character to the cache
 * not full insert
 * full replace
 * can not replace
 *
 * @param glyphIndex
 * @param character
 */
void CharacterCache::insert(unsigned int glyphIndex, Character& character) {
    assert(!isFull() || containsStale());
    if (!isFull()) {
        auto& item = cache[glyphIndex];
        item.character = character;
        item.fresh = true;
    }
    else {
        bool found = false;
        unsigned int firstStableIndex;
        for (auto& ele : cache) {
            if (!ele.second.fresh) {
                firstStableIndex = ele.first;
                found = true;
                break;
            }
        }
        assert(found);
        replace(firstStableIndex, glyphIndex, character);
    }
}

void CharacterCache::replace(unsigned int firstStaleIndex, unsigned int glyphIndex, Character& character) {
    unsigned int tmp = cache[firstStaleIndex].character.getTextureArrayIndex();
    cache.erase(firstStaleIndex);

    auto& item = cache[glyphIndex];
    character.setTextureArrayIndex(tmp);
    item.character = character;
    item.fresh = true;
}


bool CharacterCache::isFull() const {
    return cache.size() >= size;
}

bool CharacterCache::containsStale() const {
    for (const auto& ele : cache) {
        if (!ele.second.fresh) {
            return true;
        }
    }
    return false;
}

void CharacterCache::inValidate() {
    for (auto& ele : cache) {
        ele.second.fresh = false;
    }
}

Character* CharacterCache::getCharacter(unsigned int glyphIndex) {
    auto it = cache.find(glyphIndex);
    if (it != cache.end()) {
        return &it->second.character;
    }
    return nullptr;
}
















