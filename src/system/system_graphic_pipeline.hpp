#ifndef SYS_SHADER_HPP
#define SYS_SHADER_HPP

#include "ecs.h"
#include <string>

namespace Graphic {
void init(entt::registry &registry);
void update(entt::registry &registry, float dt);
} // namespace Graphic

#endif