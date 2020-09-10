#ifndef HELPER_SHADER_HPP
#define HELPER_SHADER_HPP

// clang-format off
#include <glad/glad.h>
// clang-format on
#include "component_graphic_pipeline.hpp"
#include "ecs.h"
#include <iostream>
#include "string.h"

namespace Graphic
{
  struct shader_source_loader final
      : entt::resource_loader<shader_source_loader, Graphic::ShaderSource>
  {
    std::shared_ptr<Graphic::ShaderSource> load(std::string path,
                                                Graphic::ShaderType type) const;
  };
  struct shader_loader final
      : entt::resource_loader<shader_loader, Graphic::Shader>
  {
    std::shared_ptr<Graphic::Shader> load(ShaderSource &source) const;
  };
  struct shader_program_loader final
      : entt::resource_loader<shader_program_loader, Graphic::ShaderProgam>
  {
    std::shared_ptr<Graphic::ShaderProgam>
    load(std::vector<Shader> &shaders) const;
  };

} // namespace Graphic

#endif
