#include "system_text_render.hpp"
#include "ecs.h"
#include "freefont.h"
#include "helper_text_render.hpp"
#include <iostream>
#include <sstream>
#include <string>

// clang-format off
#include <glad/glad.h>
// clang-format on
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "component_text.hpp"
#include <map>
#include "component_graphic_pipeline.hpp"
#include "system_graphic_pipeline.hpp"
#include "helper_graphic_pipeline.hpp"
namespace Graphic
{
    namespace Text
    {

        namespace
        {

            static constexpr entt::hashed_string ORTHO_CAMERA = "TEXT_ORTHO"_hs;
            static constexpr entt::hashed_string TEXT_SHADER = "TEXT_SHADER"_hs;
            static constexpr entt::hashed_string TEXT_VAO = "TEXT_VAO"_hs;
            static constexpr entt::hashed_string TEXT_VBO = "TEXT_VBO"_hs;
            entt::registry textRenderRegistry;
            FT_Library ft;
            std::unordered_map<int, Character> characters;

            void handle_error_context()
            {
                std::cout << "render text context not initialized !" << std::endl;

                throw std::invalid_argument::invalid_argument(
                    "render text context not initialized !");
            }
            void init_context(entt::registry &registry)
            {
                auto &context = textRenderRegistry.set<TextRenderContext>();
            }
            entt::hashed_string hash_character(FT_Face face, char character, int size)
            {
                std::stringstream sstm;
                sstm << face->family_name << character << size;
                entt::hashed_string hash{sstm.str().c_str()};
                return hash;
            }
            void load_character(FT_Face face, char c, int size)
            {
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                FT_Set_Pixel_Sizes(face, 0, size);
                if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                {
                    std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
                    return;
                }
                // generate texture;
                unsigned int texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer);
                // set texture options
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                auto myTexture = Texture();
                myTexture.id = texture;
                myTexture.height = face->glyph->bitmap.rows;
                myTexture.width = face->glyph->bitmap.width;
                auto sprite = Sprite{myTexture, glm::vec4(0.0f, 0.0f,
                                                          face->glyph->bitmap.width,
                                                          face->glyph->bitmap.rows)};

                // store character for later use
                Character character = {
                    texture,
                    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    face->glyph->advance.x,
                    sprite};
                entt::hashed_string hash = hash_character(face, c, size);
                characters[hash] = character;
            }
            void load_ascii_characters(FT_Face face, int size)
            {

                for (unsigned char c = 0; c < 128; c++)
                {
                    load_character(face, c, size);
                }
            }
            void init_gl()
            {

                unsigned int VAO, VBO;
                glGenVertexArrays(1, &VAO);
                glGenBuffers(1, &VBO);
                glBindVertexArray(VAO);
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                                      (void *)0);

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
                if (auto *ptr = textRenderRegistry.try_ctx<TextRenderContext>(); ptr)
                {
                    ptr->vao = VAO;
                    ptr->vbo = VBO;
                    return;
                }
                handle_error_context();
            }
            void init_freetype()
            {
                if (FT_Init_FreeType(&ft))
                {
                    std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
                }
            }
            void init_graphic()
            {
                auto text_vs_source = Graphic::load_shader_source(
                    "text_vs"_hs, "./shader/text.vect", Graphic::VertexShader);
                auto text_fs_source = Graphic::load_shader_source(
                    "text_fs"_hs, "./shader/text.frag", Graphic::FragmentShader);

                auto text_vs = Graphic::load_shader("text_vs"_hs, text_vs_source);
                auto text_fs = Graphic::load_shader("text_fs"_hs, text_fs_source);

                auto text_shader_r = Graphic::load_shader_program(
                    "text"_hs, std::vector<Graphic::Shader>{text_vs, text_fs});
                if (auto *ptr = textRenderRegistry.try_ctx<TextRenderContext>(); ptr)
                {
                    ptr->program = text_shader_r;
                    return;
                }
                handle_error_context();
            }
        }
        void init(entt::registry &registry, float w, float h)
        {
            init_context(registry);

            if (auto *ptr = textRenderRegistry.try_ctx<TextRenderContext>(); ptr)
            {
                init_graphic();
                init_freetype();
                init_gl();
                ptr->ortho = glm::ortho(0.0f, w, 0.0f, h);
                return;
            }
            handle_error_context();
        }

        std::vector<glm::vec2> calculate_render_text_vertices(int VBO, const RenderText renderText, const FT_Face &font_face)
        {
            std::string::const_iterator c;
            auto i = 0;
            auto position = renderText.position;
            std::vector<glm::vec2> vertices;
            for (c = renderText.text.begin(); c != renderText.text.end(); c++)
            {
                const float scale = renderText.scale;
                entt::hashed_string hash = hash_character(font_face, renderText.text[i], renderText.size);
                Character ch = characters[hash];
                float letterAdvance = (ch.advance >> 6) * scale;
                float xpos = (position.x + ch.bearing.x * scale);
                float ypos = position.y - (ch.size.y - ch.bearing.y) * scale;

                float w = ch.size.x * scale;
                float h = ch.size.y * scale;
                if (position.y != 0)
                {
                    ypos -= h;
                }
                auto sprite = ch.sprite;
                glm::vec2 charVectices[] = {
                    glm::vec2(xpos, ypos + h),
                    Graphic::calculate_uv(sprite, glm::vec2(0.0f, 0.0f)),
                    glm::vec2(xpos, ypos),
                    Graphic::calculate_uv(sprite, glm::vec2(0.0f, 1.0f)),
                    glm::vec2(xpos + w, ypos),
                    Graphic::calculate_uv(sprite, glm::vec2(1.0f, 1.0f)),

                    glm::vec2(xpos, ypos + h),
                    Graphic::calculate_uv(sprite, glm::vec2(0.0f, 0.0f)),
                    glm::vec2(xpos + w, ypos),
                    Graphic::calculate_uv(sprite, glm::vec2(1.0f, 1.0f)),
                    glm::vec2(xpos + w, ypos + h),
                    Graphic::calculate_uv(sprite, glm::vec2(1.0f, 0.0f)),
                };
                vertices.insert(vertices.end(), std::begin(charVectices), std::end(charVectices));
                position.x += letterAdvance;
                i++;
            }
            return vertices;
        }
        void update(entt::registry &registry, float dt)
        {

            if (auto *ptr = textRenderRegistry.try_ctx<TextRenderContext>(); ptr)
            {
                auto view = registry.view<RenderText>();
                const int VBO = ptr->vbo;
                std::vector<glm::vec2> vertices;
                for (auto &[entity, renderText] : view.each())
                {

                    auto font_face = ptr->font_cache.handle(renderText.font);
                    auto renderTextVertices = calculate_render_text_vertices(VBO, renderText, font_face);
                    vertices.insert(vertices.end(), renderTextVertices.begin(), renderTextVertices.end());
                }
                if (vertices.size() > 0)
                {
                    glBindBuffer(GL_ARRAY_BUFFER, VBO);
                    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices), &vertices[0], GL_STREAM_DRAW);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                }
                return;
            }
            handle_error_context();
        }
        void render(entt::registry &registry)
        {
            if (auto *ptr = textRenderRegistry.try_ctx<TextRenderContext>(); ptr)
            {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                auto view = registry.view<RenderText>();
                const ShaderProgam shader = ptr->program;
                const int VAO = ptr->vao;
                const int VBO = ptr->vbo;
                auto ortho = ptr->ortho;
                glUseProgram(shader.id);
                glUniformMatrix4fv(glGetUniformLocation(shader.id, "projection"),
                                   1, GL_FALSE, glm::value_ptr(ortho));

                glBindVertexArray(VAO);
                glActiveTexture(GL_TEXTURE0);
                glUniform1i(glGetUniformLocation(shader.id, "text"), 0);
                int i = 0;
                for (auto &[entity, renderText] : view.each())
                {

                    auto font_face = ptr->font_cache.handle(renderText.font);
                    glUniform3f(glGetUniformLocation(shader.id, "textColor"),
                                renderText.color.x,
                                renderText.color.y,
                                renderText.color.z);
                    std::string::const_iterator c;
                    auto position = renderText.position;
                    for (c = renderText.text.begin(); c != renderText.text.end(); c++)
                    {

                        entt::hashed_string hash = hash_character(font_face, *c, renderText.size);
                        Character ch = characters[hash];
                        glBindTexture(GL_TEXTURE_2D, ch.textureId);
                        glDrawArrays(GL_TRIANGLES, i * 6, 6);
                        i++;
                    }
                }
                glBindVertexArray(0);
                return;
            }
            handle_error_context();
        }

        void load_font(entt::hashed_string key, const std::string &path, int size)
        {
            if (auto *ptr = textRenderRegistry.try_ctx<TextRenderContext>(); ptr)
            {
                auto font_face = ptr->font_cache.load<font_face_loader>(key, ft, path);
                //TODO: dont hardcode character size
                load_ascii_characters(font_face, size);
                return;
            }
            handle_error_context();
        }

    }
}