#include "system_text_render.hpp"
#include "helper_text_render.hpp"
#include "freefont.h"
#include <string>
#include <iostream>
#include <sstream>
#include "ecs.h"
// clang-format off
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "component_text.hpp"
#include <map>

namespace Graphic
{
    namespace Text
    {
        namespace
        {
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
            entt::hashed_string hash_character(FT_Face face, char character,int size) {
                std::stringstream sstm;
                sstm << character << size;
                entt::hashed_string hash{sstm.str().c_str()};
                return hash;
            }
            void load_character(FT_Face face, char c,int size)
            {
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                FT_Set_Pixel_Sizes(face,0,size);
                if(FT_Load_Char(face,c,FT_LOAD_RENDER)) {
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
                    face->glyph->bitmap.buffer
                );
                // set texture options
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
                // store character for later use
                Character character = {
                    texture, 
                    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    face->glyph->advance.x
                };
                entt::hashed_string hash = hash_character(face,c,size);
                characters[hash] = character;
            }
            void load_ascii_characters(FT_Face face,int size) {
                  
                for (unsigned char c = 0; c < 128; c++)
                {
                    load_character(face,c,size);
                }
            }
        }
        void init(entt::registry &registry)
        {
            init_context(registry);
            //init free type
            if (FT_Init_FreeType(&ft))
            {
                std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
            }
        }
        void update(entt::registry &registry, float dt)
        {
        }
        void render(entt::registry &registry)
        {
        }
        void load_font(entt::hashed_string key, const std::string &path)
        {
            if (auto *ptr = textRenderRegistry.try_ctx<TextRenderContext>(); ptr)
            {
                auto font_face = ptr->font_cache.load<font_face_loader>(key, ft, path);
                load_ascii_characters(font_face,16);
                return;
            }
            handle_error_context();
        }
    }
}