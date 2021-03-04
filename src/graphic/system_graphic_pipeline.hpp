#pragma once
#ifndef SYS_SHADER_HPP
#define SYS_SHADER_HPP

#include "graphic/component_graphic_pipeline.hpp"
#include "graphic/component_render.hpp"
#include "ecs.h"
#include <string>

namespace Graphic
{

  namespace
  {

    struct RenderContext
    {
      entt::resource_cache<Graphic::ShaderSource> shader_source_cache;
      entt::resource_cache<Graphic::Shader> shader_cache;
      entt::resource_cache<Graphic::ShaderProgam> program_cache;
      entt::resource_cache<Graphic::Image> image_cache;
      entt::resource_cache<Graphic::Texture> texture_cache;
      std::vector<std::unique_ptr<entt::registry>> render_group_registry;
      std::unique_ptr<entt::observer> position_observer;
    };
  } // namespace
  void init(entt::registry &registry);
  void update(entt::registry &registry, float dt);
  void render(entt::registry &registry);
  entt::resource_handle<Graphic::ShaderSource>
  load_shader_source(const entt::hashed_string key, const std::string &path,
                     const Graphic::ShaderType type);

  entt::resource_handle<Graphic::Shader>
  load_shader(const entt::hashed_string key, ShaderSource &source);
  entt::resource_handle<Graphic::Image> load_image(const entt::hashed_string key,
                                                   const std::string &path,
                                                   bool flipVertically = false);
  entt::resource_handle<Graphic::Texture>
  load_texture(const entt::hashed_string key, Image &image);
  entt::resource_handle<Graphic::ShaderProgam>
  load_shader_program(const entt::hashed_string key,
                      std::vector<Graphic::Shader> &shaders);
  RenderGroupHandle create_render_group(const entt::hashed_string &shader,
                                        const entt::hashed_string &texture);
  entt::resource_handle<Graphic::ShaderProgam> get_shader_program(const entt::hashed_string key);
  entt::resource_handle<Graphic::Texture> get_texture(const entt::hashed_string key);
  void clear_render_group(RenderGroupHandle group);
  void render_group(entt::registry &registry, const RenderGroup &renderGroup);
  void render_group(entt::basic_handle<entt::entity> &group);
  void add_projection_matrix(glm::mat4 projection);
  void render_sprite(entt::registry &registry);
  void destroy(entt::registry &registry, entt::entity e);
  ProjectionMatrix projection_matrix();
} // namespace Graphic

#endif