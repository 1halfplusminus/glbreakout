#pragma once

#include "physic/system_physic.hpp"
#include "physic/component_physic.hpp"
#include "graphic/component_render.hpp"
#include "physic/helper_physic.hpp"
#include <iostream>
#include <box2d/box2d.h>
#include <unordered_map>
#include "gameplay/component_gameplay.hpp"



namespace Physic
{
    b2Vec2 gravity(0.0f, 0.0f);
    b2World world(gravity);
    float timeStep = 1.0f / 60.0f;
    int32 velocityIterations = 10;
    int32 positionIterations = 10;
    std::unordered_map<entt::entity,b2Body*> bodies;
    std::unique_ptr<entt::observer> velocity_observer;

    void init_body(entt::registry &registry) {
        auto colladable = registry.view<RigidBody,Graphic::Position,Graphic::Transform>();
        for (auto a : colladable)
        {
            if(bodies.find(a) == bodies.end()) {
                auto position = colladable.get<Graphic::Position>(a);
                auto transform = colladable.get<Graphic::Transform>(a);
                b2BodyDef bodyDef;
                bodyDef.position.Set(position.value.x, position.value.y);
                bodyDef.linearDamping = 0.0f;
                bodyDef.angularDamping = 0.01f;
                bodyDef.type = b2_staticBody;
                if(registry.has<Gameplay::Velocity>(a) || registry.has<Gameplay::Movable>(a) ) {
                    bodyDef.type = b2_dynamicBody;
                }
                bodies[a] = world.CreateBody(&bodyDef);
                if (registry.has<AABB>(a))
                {
                    auto aAABB = registry.get<AABB>(a);
                    b2PolygonShape box;
                    box.SetAsBox(transform.size.x / 2.0f,transform.size.y / 2.0f);

                    b2FixtureDef paddleShapeDef;
                    paddleShapeDef.shape = &box;
                    /* paddleShapeDef.density = 10.0f; */
                    paddleShapeDef.friction = 0.4f;
                    paddleShapeDef.restitution = 0.1f;
                    bodies[a]->CreateFixture(&paddleShapeDef);
                }

                if(registry.has<SphereCollider>(a)) {
                    auto sphereCollider = registry.get<SphereCollider>(a);
                    b2CircleShape circle;
                    circle.m_radius = sphereCollider.radius;

                    if(registry.has<Gameplay::Velocity>(a)) {
                        b2FixtureDef fixtureDef;
                        fixtureDef.shape = &circle;
                        fixtureDef.density = 1.0f;
                        fixtureDef.friction = 0.f;
                        fixtureDef.restitution = 1.0f;
                        bodies[a]->CreateFixture(&fixtureDef);
                    }
                }
 
            }
            
        }
    }

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
        velocity_observer = std::unique_ptr<entt::observer>(
            new entt::observer(registry, entt::collector.update<Gameplay::Velocity>()
                                             .where<RigidBody>()));
    }
    void update(entt::registry &registry, float dt)
    {
        registry.clear<Collision>();
        init_body(registry);
        //update rigidbody
        auto ctx = context(registry);
        auto &observer = *ctx->position_observer;
        for (auto entity : observer)
        {
           if(bodies.find(entity) != bodies.end()) {
                auto position = registry.get<Graphic::Position>(entity);
                bodies[entity]->SetTransform(b2Vec2(position.value.x,position.value.y),0.0f);
            }
        }
        auto &velocity_obs = registry.view<Gameplay::Velocity,RigidBody,Graphic::Position>();
        for(auto entity: velocity_obs) {
            auto velo = registry.get<Gameplay::Velocity>(entity);
            auto position = registry.get<Gameplay::Velocity>(entity);
            /* bodies[entity]->SetBullet(true); */
            /* bodies[entity]->ApplyLinearImpulse(b2Vec2(10,10),b2Vec2(position.value.x,position.value.y),true); */
            bodies[entity]->ApplyLinearImpulse(b2Vec2(10,10),b2Vec2(0,0),true);
            bodies[entity]->SetLinearVelocity(b2Vec2(velo.value.x,velo.value.y)); 
            registry.remove<Gameplay::Velocity>(entity);
        }
        
        world.Step(timeStep, velocityIterations, positionIterations);
        std::vector<entt::entity> toDelete;
        auto view = registry.view<Graphic::Position,RigidBody,Graphic::Transform>();
        for(auto a : view) 
        {
            auto &position = view.get<Graphic::Position>(a);
            auto &gTransform = view.get<Graphic::Transform>(a);
            auto transform = bodies[a]->GetTransform();
            if(bodies[a]->GetType() == b2_dynamicBody) {
                position.value.x = transform.p.x;
                position.value.y = transform.p.y; 
                registry.replace<Graphic::Position>(a,position);
                gTransform.rotate = -1 * bodies[a]->GetAngle();
            }
          
            /* registry.replace<Graphic::Position>(a,position); */
        }
        /* for (auto a : bodies)
        {
            if(registry.valid(a.first)) {
                registry.patch<Graphic::Position>(a.first, [a](Graphic::Position &position) {
                    auto transform = a.second->GetTransform();
                    position.value.x = transform.p.x;
                    position.value.y = transform.p.y;
                });
            } else {
                world.DestroyBody(a.second);
                toDelete.push_back(a.first);
            }
        }  */
        for (auto a : toDelete)
        {
            bodies.erase(a);
        } 
       // test
    }
} // namespace Physic