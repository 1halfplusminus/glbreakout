#pragma once
#include "ecs.h"
#include "freefont.h"
#include <glm/glm.hpp>
#include "component_graphic_pipeline.hpp"
namespace Graphic
{
    namespace Text
    {
        struct TextRenderContext
        {
            entt::resource_cache<FT_Face> font_cache;
            int vbo;
            int vao;
            ShaderProgam program;
            glm::mat4 ortho;
        };
        void init(entt::registry &registry, float w, float h);
        void update(entt::registry &registry, float dt);
        void render(entt::registry &registry);
        void load_font(const entt::hashed_string key, const std::string &path,int size);
    }

}