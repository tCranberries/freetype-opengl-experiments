// Copyright 2019 <Andrea Cognolato>
#ifndef SRC_RENDERER_H_
#define SRC_RENDERER_H_

#include <glad/glad.h>

#include <string>
#include <utility>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

#include "./face_collection.h"
#include "./shader.h"
#include "./shaping_cache.h"
#include "./state.h"
#include "./texture_atlas.h"

// dir
#include "dir/Text.h"
#include "dir/FaceCollection.h"
#include "dir/Face.h"
#include "dir/Renderer.h"


namespace renderer {
//using face_collection::AssignCodepointsFaces;
//using face_collection::FaceCollection;
using shaping_cache::CodePointsFacePair;
using shaping_cache::ShapingCache;
using state::State;
using std::array;
using std::get;
using std::pair;
using std::string;
using std::vector;
using texture_atlas::TextureAtlas;
void Render(Renderer fontRender,
        const Shader &shader,
            Text& text,
            FaceCollection& faceCollection,
            const vector<TextureAtlas *> &texture_atlases,
            GLuint VAO,
            GLuint VBO);
pair<Character, vector<unsigned char>> RenderGlyph(FT_Face face, hb_codepoint_t codepoint);

}  // namespace renderer

#endif  // SRC_RENDERER_H_
