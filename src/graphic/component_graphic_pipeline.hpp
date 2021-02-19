#ifndef COMP_SHADER_HPP
#define COMP_SHADER_HPP

#include "graphic/component_render.hpp"
#include "ecs.h"
#include "vector"
#include <glm/glm.hpp>
#include <string>

namespace Graphic
{

  enum ShaderType
  {
    VertexShader,
    FragmentShader
  };

  struct ShaderSource
  {
    std::string source;
    ShaderType type;
  };

  struct Shader
  {
    unsigned int id;
    ShaderType type;

  public:
    operator int() { return id; };
  };

  struct ShaderProgam
  {
    unsigned int id;

  public:
    operator int() { return id; };
  };
  struct VertexBuffer
  {
    unsigned int id;
  };
  struct VertexBuffers
  {
    std::vector<VertexBuffer> vertexBuffer;
  };

  struct VertexArray
  {
    unsigned int id;
  };
  struct ProjectionMatrix
  {
    glm::mat4 value;
  };
  struct VertexAttrib
  {
    int location;
    std::string name;
    int number;
    bool normalized;
  };
  struct VertexLayout
  {
    std::vector<VertexAttrib> attributes;
  };

  struct UniformInfo
  {
    std::string name;
    int location;
  };
  struct VerticesHandle
  {
    std::size_t index;
    std::size_t size;
  };
  struct RenderGroup
  {
    entt::hashed_string shader;
    entt::hashed_string texture;
  };
  struct RenderGroupHandle
  {
    int index;
  };
  struct RenderSprite
  {
    Sprite sprite;
    RenderGroupHandle group;
    glm::vec4 color;
  };
  struct Particle
  {
    glm::vec2 position, velocity;
    glm::vec4 color;
    float life;

    Particle()
        : position(0.0f), velocity(0.0f), color(1.0f), life(0.0f) {}
  };

  struct Destroy
  {
  };
} // namespace Graphic

#endif