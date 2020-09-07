#ifndef COMP_SHADER_HPP
#define COMP_SHADER_HPP

#include "ecs.h"
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
} // namespace Graphic

#endif