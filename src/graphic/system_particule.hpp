
#include "graphic/component_graphic_pipeline.hpp"
#include "graphic/component_render.hpp"
#include "ecs.h"
#include <string>

namespace Graphic
{
    namespace Particule
    {
        void init(entt::registry &registry);
        void update(entt::registry &registry, float dt);
    } // namespace Particule

} // namespace Graphic
