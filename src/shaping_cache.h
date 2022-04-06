// Copyright 2019 <Andrea Cognolato>
#ifndef SRC_SHAPING_CACHE_H_
#define SRC_SHAPING_CACHE_H_

#include <harfbuzz/hb.h>

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace shaping_cache {
    using std::pair;
    using std::string;
    using std::unordered_map;
    using std::vector;

    /**
     * vector<size_t> 代码点 对应的 mFace 在 FaceCollection 中的下标
     *
     * char:     char           char1  char2  char3
     * first:    mFace           index1 index2 index3
     * second:   glyphIndex     index1 index2 index3
     */
    typedef pair<vector<size_t>, vector<hb_codepoint_t>> CodePointsFacePair;

    /*
     * line ->  codePointsFacePair
     * 每一行 字符 对应的 代码点 及其 mFace 下标
     */
    typedef unordered_map<string, CodePointsFacePair> ShapingCache;

}  // namespace shaping_cache

#endif  // SRC_SHAPING_CACHE_H_
