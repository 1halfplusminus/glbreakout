#pragma once
#ifndef COMP_GAMEPLAY_HPP
#define COMP_GAMEPLAY_HPP
#include "ecs.h"
#include "graphic/graphic_texture.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace Gameplay
{
  constexpr entt::hashed_string player_tag = "player"_hs;
  enum class BrickType
  {
    SOLID,
    BREAKABLE
  };
  struct Duration
  {
    float value;
  };
  struct Brick
  {
    BrickType type;
    bool destroyed;
  };
  struct Velocity
  {
    glm::vec3 value;
  };
  struct Movable
  {
    glm::vec3 speed;
  };
  struct Ball
  {
    bool sticky;
    bool passThrough;
  };
  struct Level
  {
    std::vector<BrickType> bricks;
    std::vector<glm::vec2> positions;
    std::vector<glm::vec4> colors;
    unsigned int rows = 0;
    unsigned int cols = 0;
    float unit_width = 0.0f;
    float unit_height = 0.0f;
  };

  struct PowerUp
  {
    enum class Type
    {
      Chaos,
      Speed,
      Sticky,
      PassThrough,
      PadSizeIncrease,
      Confuse
    };

    Type type;
    bool active;
  };

}

#endif
// namespace Gameplay