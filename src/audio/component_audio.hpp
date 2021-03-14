#pragma once
#include "ecs.h"

namespace Audio
{
    struct Play2D
    {
        entt::hashed_string name;
        bool loopback;
    };
    struct SoundHandle
    {
        int index;
    };
    struct Sound
    {
    };
} // namespace Audio
