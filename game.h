#pragma once
#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

// clang-format off
#include <glad/glad.h>
// clang-format on
#include "ecs.h"
#include <glm/glm.hpp>

namespace Game
{
  constexpr entt::hashed_string WORLD_TAG = "world_tag"_hs;
  struct World
  {
    float deltaTime;
    glm::vec2 viewport;
  };
  namespace
  {
    struct GameContext
    {
      float delatTime;
    };
  } // namespace
  void init(Registry &registry, float w, float h);
  void render(Registry &registry);
  // game loop
  void processInput(Registry &registry, int action);
  void updateSim(Registry &registry, float dt);
  void update(Registry &registry, float dt);
  entt::basic_handle<entt::entity> world(Registry &registry);
}; // namespace Game

#endif