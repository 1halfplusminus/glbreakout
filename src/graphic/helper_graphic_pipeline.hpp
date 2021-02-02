#ifndef HELPER_SHADER_HPP
#define HELPER_SHADER_HPP

// clang-format off
#include <glad/glad.h>
// clang-format on
#include "graphic/component_graphic_pipeline.hpp"
#include "ecs.h"
#include <iostream>
#include "string.h"
#include "graphic/component_render.hpp"
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
  glm::mat4 create_model_matrix(const Position &position, const Transform &transform);
  glm::vec2 calculate_uv(const Sprite &sprite, const VertexData &vertex);
  glm::vec2 calculate_uv(const Sprite &sprite, glm::vec2 vertice);
} // namespace Graphic

#endif
