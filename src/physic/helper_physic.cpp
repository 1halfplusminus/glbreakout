#pragma once

#include "physic/helper_physic.hpp"

namespace Physic
{
    bool intersect(AABB a, AABB b)
    {
        return (a.lowerBound.x <= b.upperBound.x && a.upperBound.x >= b.lowerBound.x) &&
               (a.lowerBound.y <= b.upperBound.y && a.upperBound.y >= b.lowerBound.y);
    }
    bool intersect(AABB a, glm::vec3 position, float radius)
    {
        auto clamp = glm::max(a.lowerBound, glm::min(position, a.upperBound));
        auto distance = glm::distance(clamp, position);
        return distance < radius;
    }
} // namespace Physic