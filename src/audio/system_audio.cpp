#pragma once
#include <iostream>
#include "system_audio.hpp"

namespace Audio
{
    namespace
    {
        entt::registry audioRegistry;
        void handle_error_context()
        {
            std::cout << "audio context not initialized !" << std::endl;
            throw std::invalid_argument::invalid_argument(
                "audio context not initialized !");
        }
        void init_context(entt::registry &registry)
        {
            auto &context = audioRegistry.set<AudioContext>();
        }
    }
    void load_sound(const entt::hashed_string key, const std::string &path)
    {
        if (auto *ptr = audioRegistry.try_ctx<AudioContext>(); ptr)
        {
            auto &buffer = ptr->sound_buffer_cache.load<sound_buffer_loader>(key, path).get();
            ptr->sound_cache.load<sound_loader>(key, buffer);
            return;
        }
        handle_error_context();
    }

    void init(entt::registry &registry)
    {
        init_context(registry);
    }
    void update(entt::registry &registry, float dt)
    {
        if (auto *ptr = audioRegistry.try_ctx<AudioContext>(); ptr)
        {
            auto view = registry.view<Play2D>();
            for (auto [entity, play2D] : view.each())
            {
                auto &sound = ptr->sound_cache.handle(play2D.name).get();
                if (sound.getStatus() != sound.Playing)
                {
                    sound.play();
                }
            }
            return;
        }
        handle_error_context();
    }
} // namespace Audio
