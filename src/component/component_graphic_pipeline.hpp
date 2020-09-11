#ifndef COMP_SHADER_HPP
#define COMP_SHADER_HPP

#include "ecs.h"
#include <string>
#include <glm/glm.hpp>
#include "vector"

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
  };

  struct ShaderProgam
  {
    unsigned int id;
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

} // namespace Graphic

#endif