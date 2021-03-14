#pragma once
#include "ecs.h"
#include "component_audio.hpp"
#include "audio_sound.hpp"

namespace Audio
{
    struct AudioContext
    {
        entt::resource_cache<sf::Sound> sound_cache;
        entt::resource_cache<sf::SoundBuffer> sound_buffer_cache;
    };
    void init(entt::registry &registry);
    void load_sound(const entt::hashed_string key, const std::string &path);
    void update(entt::registry &registry, float dt);
} // namespace Audio
