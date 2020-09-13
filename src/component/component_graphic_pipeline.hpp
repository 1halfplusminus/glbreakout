#ifndef COMP_SHADER_HPP
#define COMP_SHADER_HPP

#include "component/component_render.hpp"
#include "ecs.h"
#include "vector"
#include <glm/glm.hpp>
#include <string>

namespace Graphic {

enum ShaderType { VertexShader, FragmentShader };

struct ShaderSource {
  std::string source;
  ShaderType type;
};

struct Shader {
  unsigned int id;
  ShaderType type;
};

struct ShaderProgam {
  unsigned int id;
};
struct VertexBuffer {
  unsigned int id;
};
struct VertexBuffers {
  std::vector<VertexBuffer> vertexBuffer;
};

struct VertexArray {
  unsigned int id;
};
struct ProjectionMatrix {
  glm::mat4 value;
};
struct VertexAttrib {
  int location;
  std::string name;
  int number;
  bool normalized;
};
struct VertexLayout {
  std::vector<VertexAttrib> attributes;
};

struct UniformInfo {
  std::string name;
  int location;
};
struct VerticesHandle {
  std::size_t index;
  std::size_t size;
};
struct RenderGroup {
  entt::hashed_string shader;
  entt::hashed_string texture;
};
struct RenderGroupHandle {
  int index;
};
struct RenderSprite {
  Sprite sprite;
  RenderGroupHandle group;
  glm::vec4 color;
};
} // namespace Graphic

#endif