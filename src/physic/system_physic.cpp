#pragma once

#include "physic/system_physic.hpp"
#include "physic/component_physic.hpp"
#include "graphic/component_render.hpp"
#include "physic/helper_physic.hpp"
#include <iostream>

namespace Physic
{

    struct CheckCollision
    {

        entt::registry &registry;
        CheckCollision(entt::registry &r) : registry{r} {}
        void operator()()
        {
            auto colladable = registry.view<RigidBody>();
            for (auto a : colladable)
            {
                for (auto b : colladable)
                {
                    if (a != b)
                    {
                        auto aRigidBody = colladable.get<RigidBody>(a);
                        auto bRigidBody = colladable.get<RigidBody>(b);
                        auto sameCategory = aRigidBody.collidWith & bRigidBody.category;
                        if (sameCategory && registry.any_of<AABB>(a) && registry.any_of<AABB>(b))
                        {
                            auto aAABB = registry.get<AABB>(a);
                            auto bAABB = registry.get<AABB>(b);
                            if (Physic::intersect(aAABB, bAABB))
                            {
                                registry.emplace_or_replace<Collision>(a, a, b);
                            }
                        }
                        else if (sameCategory && registry.any_of<AABB>(a) && registry.any_of<SphereCollider>(b))
                        {
                            auto aAABB = registry.get<AABB>(a);
                            auto bSphere = registry.get<SphereCollider>(b);
                            auto bPosition = registry.get<Graphic::Position>(b);
                            if (Physic::intersect(aAABB, bPosition.value, bSphere.radius))
                            {
                                registry.emplace_or_replace<Collision>(a, a, b);
                            }
                        }
                    }
                }
            }
            return;
        };
    };

    namespace
    {
        PhysicContext *context(Registry &registry)
        {
            if (auto *ptr = registry.try_ctx<PhysicContext>(); ptr)
            {
                return ptr;
            }
            std::cout << "physic context not initialized !";
            exit(1);
        }
    } // namespace
    void init(entt::registry &registry)
    {
        auto &context = registry.set<PhysicContext>();
        context.position_observer = std::unique_ptr<entt::observer>(
            new entt::observer(registry, entt::collector.update<Graphic::Position>()
                                             .where<RigidBody>()));
    }
    void update(entt::registry &registry, float dt)
    {
        registry.clear<Collision>();
        //update rigidbody
        auto ctx = context(registry);
        auto &observer = *ctx->position_observer;
        for (auto entity : observer)
        {
            if (registry.all_of<AABB, Graphic::Transform>(entity))
            {
                registry.patch<AABB>(entity, [&registry, entity](AABB &aabb) {
                    auto transform = registry.get<Graphic::Transform>(entity);
                    auto position = registry.get<Graphic::Position>(entity);
                    aabb.lowerBound = position.value;
                    aabb.upperBound = position.value + glm::vec3(transform.size, 0);
                });
            }
        }
        CheckCollision{registry}();
    }
} // namespace Physic