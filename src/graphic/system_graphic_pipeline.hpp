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
      entt::resource_cache<ShaderSource> shader_source_cache;
      entt::resource_cache<Shader> shader_cache;
      entt::resource_cache<ShaderProgam> program_cache;
      entt::resource_cache<Image> image_cache;
      entt::resource_cache<Texture> texture_cache;
      std::vector<std::unique_ptr<entt::registry>> render_group_registry;
      std::unique_ptr<entt::observer> position_observer;
      std::unique_ptr<entt::observer> sprite_observer;
    };
  } // namespace
  void init(entt::registry &registry);
  void update(entt::registry &registry, float dt);
  void render(entt::registry &registry);
  entt::resource_handle<ShaderSource>
  load_shader_source(const entt::hashed_string key, const std::string &path,
                     const ShaderType type);

  entt::resource_handle<Shader>
  load_shader(const entt::hashed_string key, ShaderSource &source);
  entt::resource_handle<Image> load_image(const entt::hashed_string key,
                                          const std::string &path,
                                          bool flipVertically = false);
  entt::resource_handle<Texture>
  load_texture(const entt::hashed_string key, Image image);
  entt::resource_handle<ShaderProgam>
  load_shader_program(const entt::hashed_string key,
                      std::vector<Shader> &shaders);
  RenderGroupHandle create_render_group(const entt::hashed_string &shader,
                                        const entt::hashed_string &texture);
  entt::resource_handle<ShaderProgam> get_shader_program(const entt::hashed_string key);
  entt::resource_handle<Texture> get_texture(const entt::hashed_string key);
  void clear_render_group(RenderGroupHandle group);
  void render_group(entt::registry &registry, const RenderGroup &renderGroup);
  void render_group(entt::basic_handle<entt::entity> &group);
  void add_projection_matrix(glm::mat4 projection);
  void render_sprite(entt::registry &registry);
  void destroy(entt::registry &registry, entt::entity e);
  ProjectionMatrix projection_matrix();
} // namespace Graphic

#endif