
#pragma once
#ifndef SYS_POSTPROCESSING_HPP
#define SYS_POSTPROCESSING_HPP
#include "ecs.h"

namespace Graphic
{
    namespace PostProcessing
    {
        void init(entt::registry &registry, unsigned int w, unsigned int h, const std::string vertexShader, const std::string fragmentShader);
        void update(entt::registry &registry, float dt);
        void render(entt::registry &registry);
        void active_effect(entt::hashed_string key);
        void desactive_effect();
        void desactive_effect(entt::hashed_string key);
        void load_effect(entt::hashed_string key, const std::string &vertexShaderPath, const std::string &fragShaderPath);

    }
} // namespace Graphic
#endif