#include <glm/glm.hpp>
#include "ecs.h"

namespace Graphic
{
    namespace Text
    {
        struct RenderText
        {
            glm::vec2 position;
            std::string text;
            glm::vec3 color;
            float scale = 1.0f;
            entt::hashed_string font;
            int size;
        };
        struct Character
        {
            unsigned int textureId;
            glm::ivec2 size;
            glm::ivec2 bearing;
            unsigned int advance;
            Sprite sprite;
        };
    }
}
