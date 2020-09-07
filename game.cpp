

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
void Game::update(Registry &registry, float dt) {
  Graphic::update(registry, dt);
}
void Game::init(Registry &registry, float w, float h) {
  Graphic::init(registry);
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
  program_cache.load<Graphic::shader_program_loader>(
      "main"_hs,
      std::vector<Graphic::Shader>{shader_cache.handle("main_vs"_hs),
                                   shader_cache.handle("main_fs"_hs)});

  entt::resource_cache<Graphic::Image> image_cache;
  image_cache.load<Graphic::image_loader>("awesomeface"_hs,
                                          "./texture/awesomeface.png", false);

  entt::resource_cache<Graphic::Texture> texture_cache;
  texture_cache.load<Graphic::texture_image_loader>(
      "awesomeface"_hs, image_cache.handle("awesomeface"_hs));

  // create player
  auto player = registry.create();

  registry.emplace<Graphic::Sprite>(
      player, std::vector<float>({0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
                                  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

                                  0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
                                  1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f}));
  registry.emplace<Graphic::Texture>(
      player, texture_cache.handle("awesomeface"_hs).get().id);
  registry.emplace<Graphic::ShaderProgam>(
      player, program_cache.handle("main"_hs).get().id);

  registry.emplace<Graphic::Position>(player, glm::vec3(200.0f, 200.0f, 0.0f));
  registry.emplace<Graphic::Transform>(player, 45.0f, glm::vec2(300.0f, 400.0f),
                                       glm::vec3(0.0f, 1.0f, 0.0f));
  // create camera
  auto camera = registry.create();
  registry.emplace<Graphic::ProjectionMatrix>(
      camera, glm::ortho(0.0f, static_cast<float>(w), static_cast<float>(h),
                         0.0f, -1.0f, 1.0f));
}