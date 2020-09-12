#ifndef SYS_SHADER_HPP
#define SYS_SHADER_HPP

#include "ecs.h"
#include <string>
#include "component/component_render.hpp"
#include "component/component_graphic_pipeline.hpp"

namespace Graphic
{
    struct RenderGroupHandle
    {
        int index;
    };
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
        };
    } // namespace
    void init(entt::registry &registry);
    void update(entt::registry &registry, float dt);
    entt::resource_handle<Graphic::ShaderSource> load_shader_source(const entt::hashed_string key, const std::string &path,
                                                                    const Graphic::ShaderType type);

    entt::resource_handle<Graphic::Shader> load_shader(const entt::hashed_string key, ShaderSource &source);
    entt::resource_handle<Graphic::Image> load_image(const entt::hashed_string key, const std::string &path,
                                                     bool flipVertically = false);
    entt::resource_handle<Graphic::Texture> load_texture(const entt::hashed_string key, Image &image);
    entt::resource_handle<Graphic::ShaderProgam> load_shader_program(const entt::hashed_string key, std::vector<Graphic::Shader> &shaders);
    RenderGroupHandle create_render_group(const entt::hashed_string &shader, const entt::hashed_string &texture);
    void clear_render_group(RenderGroupHandle group);
    entt::basic_handle<entt::entity> create_sprite(RenderGroupHandle &group,
                                                   const Sprite &sprite,
                                                   const glm::vec3 &position,
                                                   const glm::vec2 &size,
                                                   const float &rotate,
                                                   const glm::vec4 &color);
    void render_group(entt::registry &registry, RenderGroup &renderGroup);
    void render_group(entt::basic_handle<entt::entity> &group);
    void add_projection_matrix(glm::mat4 projection);
    void render_sprite(entt::registry &registry);
} // namespace Graphic

#endif