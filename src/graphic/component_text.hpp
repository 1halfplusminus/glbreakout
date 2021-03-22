#pragma once
#include <glm/glm.hpp>
#include "ecs.h"

namespace Graphic
{
    namespace Text
    {
        struct RenderText
        {
            glm::vec4 position;
            std::string text;
        };
        struct Character
        {
            unsigned int textureId;
            glm::ivec2 size;
            glm::ivec2 bearing;
            unsigned int advance;
            entt::hashed_string font;
            int fontSize;
        };
    }
}
