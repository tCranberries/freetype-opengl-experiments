// Copyright 2019 <Andrea Cognolato>
// TODO(andrea): subpixel positioning (si puo' fare con freetype? lo fa gia?)
// TODO(andrea): statically link glfw, harfbuzz; making sure that both are
// TODO(andrea): benchmark startup time (500ms on emoji file, wtf)
// compiled in release mode

// 👚🔇🐕🏠 📗🍢💵📏🐁🌓 💼🐦👠
// 👚🔇🐕🏠 📗🍢💵📏🐁🌓 💼🐦👠

// glad - OpenGL loader
#include <glad/glad.h>
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_LCD_FILTER_H

#include <GLFW/glfw3.h>

// HarfBuzz
#include <harfbuzz/hb.h>
// HarfBuzz FreeTpe
#include <harfbuzz/hb-ft.h>

#include <fstream>
#include <unordered_map>
#include <vector>
#include <unistd.h>

// glm - OpenGL mathematics
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

#include "./callbacks.h"
#include "./constants.h"
#include "./renderer.h"
#include "./shader.h"
#include "./state.h"
#include "./texture_atlas.h"
#include "./util.h"
#include "./window.h"

// dir
#include "dir/Text.h"
#include "dir/FaceCollection.h"
#include "dir/Face.h"

namespace lettera {
    //using face_collection::FaceCollection;
    //using face_collection::LoadFaces;
    using renderer::Render;
    using shaping_cache::CodePointsFacePair;
    using shaping_cache::ShapingCache;
    using state::State;
    using std::get;
    using std::make_pair;
    using std::make_tuple;
    using std::pair;
    using std::string;
    using std::tuple;
    using std::unordered_map;
    using std::vector;
    using texture_atlas::TextureAtlas;
    using window::Window;

    GLuint VAO, VBO;

    // https://blog.csdn.net/zjz520yy/article/details/83000096
    /*
     * 任何的OpenGL接口调用都必须在初始化GLAD库后才可以正常访问。如果成功的话，该接口将返回GL_TRUE，否则就会返回GL_FALSE
     */
    void InitOpenGL() {
        // Check that glad worked
        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
            fprintf(stderr, "glad failed to load OpenGL loader\n");
            exit(EXIT_FAILURE);
        }

        // Enable debug output
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(util::GLDebugMessageCallback, nullptr);
    }

    int main(int argc UNUSED, char **argv) {

        Window window(kInitialWindowWidth, kInitialWindowHeight, kWindowTitle,
                      callbacks::KeyCallback, callbacks::ScrollCallback,
                      callbacks::ResizeCallback);
        State state(kInitialWindowWidth, kInitialWindowHeight, kLineHeight,
                    kInitialLine);

        InitOpenGL();

        // Compile and link the shaders
        Shader shader("/home/wzw/CLionProjects/freetype-opengl-experiments/src/shaders/text.vert", "/home/wzw/CLionProjects/freetype-opengl-experiments/src/shaders/text.frag");
        shader.use();

        callbacks::glfw_user_pointer_t glfw_user_pointer;
        glfwSetWindowUserPointer(window.window, &glfw_user_pointer);

        glfw_user_pointer.shader_program_id = shader.programId;
        glfw_user_pointer.state = &state;

        // https://stackoverflow.com/questions/48491340/use-rgb-texture-as-alpha-values-subpixel-font-rendering-in-opengl
        // TODO(andrea): understand WHY it works, and if this is an actual solution,
        // then write a blog post
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC1_COLOR, GL_ONE_MINUS_SRC1_COLOR);

        // Set the viewport
        glViewport(0, 0, kInitialWindowWidth, kInitialWindowHeight);

        // Disable byte-alignment restriction (our textures' size is not a multiple of 4)
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Init Vertex Array Object (VAO)
        // VAO （ Vertex Array Object ）是OpenGL用来处理顶点数据的一个缓冲区对象，它不能单独使用，都是结合VBO来一起使用的
        // https://blog.csdn.net/csxiaoshui/article/details/53197527
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        // Init Vertex Buffer Object (VBO)
        glGenBuffers(1, &VBO);
        glBindVertexArray(0);

        // Init projection matrix
        glm::mat4 projection =
                glm::ortho(0.0f, static_cast<GLfloat>(kInitialWindowWidth), 0.0f,
                           static_cast<GLfloat>(kInitialWindowHeight));
        glUniformMatrix4fv(glGetUniformLocation(shader.programId, "projection"), 1,
                           GL_FALSE, glm::value_ptr(projection));

        // !-------------  opengl end


        const string fontArabic = "/data0/sx-render-service/workspace/fonts/Mirza-SemiBold.ttf";
        const string fontEmoji = "/home/wzw/CLionProjects/freetype-opengl-experiments/assets/fonts/NotoColorEmoji.ttf";
        const string fontJapan = "/home/wzw/CLionProjects/freetype-opengl-experiments/assets/japan/MS Mincho.TTF";
        const string fontLatin = "/home/wzw/CLionProjects/freetype-opengl-experiments/assets/latin/UbuntuMono.ttf";
        const string fontCh = "/home/wzw/CLionProjects/freetype-opengl-experiments/assets/ch/Alibaba-PuHuiTi-Bold.otf";
        const string fontThai = "/home/wzw/CLionProjects/freetype-opengl-experiments/assets/thai/thai.ttf";
        const string fontKorean = "/home/wzw/CLionProjects/freetype-opengl-experiments/assets/korean/malgun.ttf";
        const string fontRuss = "/home/wzw/CLionProjects/freetype-opengl-experiments/assets/russ/Russian.ttf";

        vector<string> face_names;
        face_names.push_back(fontLatin);
        face_names.push_back(fontEmoji);
        face_names.push_back(fontCh);
        face_names.push_back(fontKorean);
        face_names.push_back(fontArabic);
        face_names.push_back(fontJapan);
        face_names.push_back(fontThai);



        const unsigned int fontPixelWidth = 50;
        const unsigned int fontPixelHeight = fontPixelWidth;
        bool isSpaceDecideNextLine = true;
        bool isBold = false;
        bool isItalic = false;
        double gradient = 0.6;
        double lineScale = 1.35;
        int kerning = 0;

        Text text(argv[1],
                  TextDirection::LTR,
                  TextLanguage::en,
                  fontPixelWidth,
                  fontPixelHeight,
                  isSpaceDecideNextLine,
                  isBold,
                  isItalic,
                  gradient,
                  lineScale,
                  kerning);

        glfw_user_pointer.lines = &text.getLines();

        FaceCollection faceCollection(face_names, fontPixelWidth, fontPixelHeight);
        faceCollection.loadFaces();
        faceCollection.assignCodepointsFaces(text);

        Renderer fontRenderer;

        // TODO(andrea): make this support multiple fonts
        // And the texture atlases  monochrome 单色
        TextureAtlas monochrome_texture_atlas(
                (int)faceCollection.getFaces().at(0).getWidth() + 40,
                (int)faceCollection.getFaces().at(0).getHeight() + 40,
                shader.programId,
                "monochromatic_texture_array",
                GL_RGB8,
                GL_RGB,
                0);

        TextureAtlas colored_texture_atlas(
                (int)faceCollection.getFaces().at(1).getWidth(),
                (int)faceCollection.getFaces().at(1).getHeight(),
                shader.programId,
                "colored_texture_array",
                GL_RGBA8,
                GL_BGRA,
                1);


        vector<TextureAtlas *> texture_atlases;
        texture_atlases.push_back(&monochrome_texture_atlas);
        texture_atlases.push_back(&colored_texture_atlas);

        while (!glfwWindowShouldClose(window.window)) {
            glfwWaitEvents();

            auto t1 = glfwGetTime();

            Render(fontRenderer, shader, text, faceCollection, texture_atlases, VAO, VBO);

            auto t2 = glfwGetTime();
            printf("Rendering lines took %f ms (%3.0f fps/Hz)\n", (t2 - t1) * 1000, 1.f / (t2 - t1));
            // Swap buffers when drawing is finished
            glfwSwapBuffers(window.window);

            sleep(10);
        }

        return 0;
    }
}  // namespace lettera

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage %s FILE\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    lettera::main(argc, argv);

    return 0;
}
