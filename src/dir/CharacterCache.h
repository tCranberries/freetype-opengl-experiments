//
// Created by wzw on 2022/3/23.
//

#ifndef OPENGL_CHARACTERCACHE_H
#define OPENGL_CHARACTERCACHE_H

#include <unordered_map>

#include "Character.h"

/**
 * 目前是一行字符 1024
 */
class CharacterCache {
private:

    // texture depth  cache count
    static unsigned int size;

    typedef struct {
        Character character;
        bool fresh;
    } CacheElementT;

    /*
     * glyphIndex -> character
     */
    std::unordered_map<unsigned int, CacheElementT> cache;
public:

    CharacterCache();

    ~CharacterCache();

    /**
     * insert character to the cache
     * not full insert
     * full replace
     * can not replace
     *
     * @param glyphIndex
     * @param character
     */
    void insert(unsigned int glyphIndex, Character& character);

    void replace(unsigned int firstStaleIndex, unsigned int glyphIndex, Character& character);

    bool isFull() const;

    bool containsStale() const;

    void inValidate();

    Character* getCharacter(unsigned int glyphIndex);

};



#endif //OPENGL_CHARACTERCACHE_H


















