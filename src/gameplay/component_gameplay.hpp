#include "ecs.h"

namespace Gameplay
{
    struct Level
    {
        std::vector<entt::entity> bricks;
    };
    enum class BrickType
    {
        SOLID,
        BREAKABLE
    };
    struct Brick
    {
        BrickType type;
    };

} // namespace Gameplay