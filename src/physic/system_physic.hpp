#pragma once
#include "ecs.h"

namespace Physic
{
    namespace
    {
        struct PhysicContext
        {
            std::unique_ptr<entt::observer> position_observer;
        };
    } // namespace
    void init(entt::registry &registry);
    void update(entt::registry &registry, float dt);
} // namespace Physic
