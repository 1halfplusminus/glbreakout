
#include "ecs.h"

namespace Graphic
{
    namespace PostProcessing
    {
        void init(entt::registry &registry, unsigned int w, unsigned int h, const std::string vertexShader, std::string fragmentShader);
        void update(entt::registry &registry, float dt);
        void render(entt::registry &registry);
    }
} // namespace Graphic
