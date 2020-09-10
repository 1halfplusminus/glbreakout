#include "ecs.h"
#include "component_gameplay.hpp"
#include <sstream>
#include <fstream>
#include <iostream>

namespace Gameplay
{
    struct level_loader final
        : entt::resource_loader<level_loader, Level>
    {
        std::shared_ptr<Level>
        load(std::string &filePath) const;
    };
} // namespace Gameplay