#pragma once
#include "ecs.h"
#include "freefont.h"
namespace Graphic
{
    namespace Text
    {
        struct TextRenderContext
        {
            entt::resource_cache<FT_Face> font_cache;
        };
        void init(entt::registry &registry);
        void update(entt::registry &registry, float dt);
        void render(entt::registry $registry);
        void load_font(const entt::hashed_string key, const std::string &path);
    }

}