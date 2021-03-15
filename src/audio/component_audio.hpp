#pragma once
#include "ecs.h"
#include <chrono>

namespace Audio
{
    struct Play2D
    {
        entt::hashed_string name;
        bool loopback = false;
        int offset;
        bool played = false;
        Play2D(entt::hashed_string _name, bool _loopback, int _offset = 0)
        {
            auto sec = std::chrono::seconds(1);
            name = _name;
            loopback = _loopback;
            offset = _offset;
        };
    };
    struct SoundHandle
    {
        int index;
    };
    struct Sound
    {
    };
} // namespace Audio
