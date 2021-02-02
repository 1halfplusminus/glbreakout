#ifndef COMP_RENDER_HPP
#define COMP_RENDER_HPP

#include "graphic/graphic_texture.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace Graphic
{

  struct VertexData
  {
    glm::vec4 vertice;
    glm::vec2 uv;
    glm::mat4 transform;
    glm::vec4 color;
    entt::entity sprite;
  };
  struct Vertices
  {
    std::vector<VertexData> vertices;
    Vertices(){};
    Vertices(std::vector<VertexData> &v) : vertices(v){};
  };
  struct Sprite
  {
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

  struct Target
  {
    entt::entity target;
  };
} // namespace Graphic

#endif