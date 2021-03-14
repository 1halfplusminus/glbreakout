#pragma once
#include "ecs.h"
#include "component_audio.hpp"
#include <SFML/Audio.hpp>
namespace Audio
{
    struct sound_loader final
        : entt::resource_loader<sound_loader, sf::Sound>
    {
        std::shared_ptr<sf::Sound> load(const sf::SoundBuffer &buffer) const;
    };
    struct sound_buffer_loader final
        : entt::resource_loader<sound_buffer_loader, sf::SoundBuffer>
    {
        std::shared_ptr<sf::SoundBuffer> load(const std::string &path) const;
    };
} // namespace Audio
