#ifndef ECS_H_INCLUDED
#define ECS_H_INCLUDED

#include <entt/entt.hpp>

using Registry = entt::registry;
using Entity = Registry::entity_type;
using namespace entt::literals;
static const Entity NULL_ENTITY = entt::null;

#endif