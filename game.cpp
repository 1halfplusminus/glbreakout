

#include "game.h"
#include "component/component_graphic_pipeline.hpp"
#include "component/component_render.hpp"
#include "graphic/graphic_image.hpp"
#include "graphic/graphic_texture.hpp"
#include "helper/helper_graphic_pipeline.hpp"
#include "system/system_graphic_pipeline.hpp"
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void Game::processInput(GLFWwindow *window, Registry &registry, float dt) {}
void Game::render(Registry &registry) {}
void Game::update(Registry &registry, float dt)
{
    Graphic::update(registry, dt);
}
void Game::init(Registry &registry, float w, float h)
{

    Graphic::init(registry);

    // load ressource
    entt::resource_cache<Graphic::ShaderSource> shader_source_cache;
    shader_source_cache.load<Graphic::shader_source_loader>(
        "main_vs"_hs, "./shader/main.vect", Graphic::VertexShader);
    shader_source_cache.load<Graphic::shader_source_loader>(
        "main_fs"_hs, "./shader/main.frag", Graphic::FragmentShader);

    entt::resource_cache<Graphic::Shader> shader_cache;
    shader_cache.load<Graphic::shader_loader>(
        "main_vs"_hs, shader_source_cache.handle("main_vs"_hs));
    shader_cache.load<Graphic::shader_loader>(
        "main_fs"_hs, shader_source_cache.handle("main_fs"_hs));

    entt::resource_cache<Graphic::ShaderProgam> program_cache;
    auto &main_shader_r = program_cache.load<Graphic::shader_program_loader>(
        "main"_hs,
        std::vector<Graphic::Shader>{shader_cache.handle("main_vs"_hs),
                                     shader_cache.handle("main_fs"_hs)});

    auto main_shader = registry.create();

    entt::resource_cache<Graphic::Image> image_cache;
    image_cache.load<Graphic::image_loader>("atlas"_hs,
                                            "./texture/atlas.png", false);

    entt::resource_cache<Graphic::Texture> texture_cache;
    auto &atlas = texture_cache.load<Graphic::texture_image_loader>(
                                   "atlas"_hs, image_cache.handle("atlas"_hs))
                      .get();

    // create player
    for (int i = 0; i < 10000; i++)
    {
        auto player = registry.create();

        registry.emplace<Graphic::Sprite>(
            player, std::vector<float>({0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

                                        0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f}),
            atlas,
            glm::vec4(0, 640, 512, 128));

        registry.emplace<Graphic::Material>(
            player, main_shader_r.get().id, glm::vec3(0.0f, 1.0f, 0.0f));
        registry.emplace<Graphic::Position>(player, glm::vec3(200.0f, 200.0f, 0.0f));
        registry.emplace<Graphic::Transform>(player, 45.0f, glm::vec2(512.0f, 128.0f));
    }
    // create camera
    auto camera = registry.create();
    registry.emplace<Graphic::ProjectionMatrix>(
        camera, glm::ortho(0.0f, static_cast<float>(w), static_cast<float>(h),
                           0.0f, -1.0f, 1.0f));
}