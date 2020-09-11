#ifndef SYS_SHADER_HPP
#define SYS_SHADER_HPP

#include "ecs.h"
#include <string>
#include "component/component_render.hpp"
#include "component/component_graphic_pipeline.hpp"

namespace Graphic
{
    struct RenderGroup
    {
        entt::hashed_string material;
        entt::hashed_string texture;
    };
    struct RenderContext
    {
        entt::resource_cache<Graphic::ShaderSource> shader_source_cache;
        entt::resource_cache<Graphic::Shader> shader_cache;
        entt::resource_cache<Graphic::ShaderProgam> program_cache;
        entt::resource_cache<Graphic::Image> image_cache;
        entt::resource_cache<Graphic::Texture> texture_cache;
    };
    void init(entt::registry &registry);
    void update(entt::registry &registry, float dt);
    void render_sprite(entt::registry &registry);
    void create_sprite(Sprite sprite, Material mat, Transform trans);
    entt::resource_handle<Graphic::ShaderSource> load_shader_source(const entt::hashed_string key, const std::string &path,
                                                                    const Graphic::ShaderType type);

    entt::resource_handle<Graphic::Shader> load_shader(const entt::hashed_string key, ShaderSource &source);
    entt::resource_handle<Graphic::Image> load_image(const entt::hashed_string key, const std::string &path,
                                                     bool flipVertically = false);
    entt::resource_handle<Graphic::Texture> load_texture(const entt::hashed_string key, Image &image);
    entt::resource_handle<Graphic::ShaderProgam> load_shader_program(const entt::hashed_string key, std::vector<Graphic::Shader> &shaders);
    entt::basic_handle<entt::entity> create_render_groupe(const RenderGroup &group);

} // namespace Graphic

#endif