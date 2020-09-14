#pragma once

#include "physic/component_physic.hpp"

namespace Physic
{
    bool isPointInsideAABB(glm::vec3 point, AABB box);
    bool intersect(AABB box1, AABB box2);
    bool intersect(AABB a, glm::vec3 position, float radius);
} // namespace Physic