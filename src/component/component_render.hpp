#ifndef COMP_RENDER_HPP
#define COMP_RENDER_HPP

#include <glm/glm.hpp>
#include <vector>
#include "graphic/graphic_texture.hpp"

namespace Graphic
{

  struct Sprite
  {
    std::vector<float> vertices;
    Texture texture;
    glm::vec4 rect;
  };

  struct Position
  {
    glm::vec3 value;
  };

  struct Transform
  {
    float rotate;
    glm::vec2 size;
  };

} // namespace Graphic

#endif