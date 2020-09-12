#pragma once
#include "ecs.h"
#include "graphic/graphic_texture.hpp"
#include <vector>
#include <glm/glm.hpp>
namespace Gameplay
{

    enum class BrickType
    {
        SOLID,
        BREAKABLE
    };
    struct Brick
    {
        BrickType type;
    };
    struct Level
    {
        std::vector<BrickType> bricks;
        std::vector<glm::vec2> positions;
        std::vector<glm::vec4> colors;
        unsigned int rows = 0;
        unsigned int cols = 0;
        float unit_width = 0.0f;
        float unit_height = 0.0f;
    };
} // namespace Gameplay