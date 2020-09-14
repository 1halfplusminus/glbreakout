#pragma once
#include <glm/glm.hpp>
#include "ecs.h"

namespace Physic
{
    struct Collision
    {
        entt::entity a;
        entt::entity b;
    };
    struct RigidBody
    {
        int category;
        int collidWith;
    };
    struct AABB
    {
        glm::vec3 upperBound;
        glm::vec3 lowerBound;
    };
    struct SphereCollider
    {
        float radius;
    };
} // namespace Physic