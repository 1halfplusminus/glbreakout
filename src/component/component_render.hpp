#ifndef COMP_RENDER_HPP
#define COMP_RENDER_HPP

#include <glm/glm.hpp>
#include <vector>

namespace Graphic {

struct Sprite {
  std::vector<float> vertices;
};

struct Position {
  glm::vec3 value;
};

struct Transform {
  float rotate;
  glm::vec2 size;
  glm::vec3 color;
};

struct BufferIndex {
  unsigned int id;
};

struct ProjectionMatrix {
  glm::mat4 value;
};

} // namespace Graphic

#endif