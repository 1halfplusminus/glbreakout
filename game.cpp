

#include "game.h"
#include "graphic/component_graphic_pipeline.hpp"
#include "graphic/component_render.hpp"
#include "gameplay/component_gameplay.hpp"
#include "gameplay/helper_gameplay.hpp"
#include "graphic/graphic_image.hpp"
#include "graphic/graphic_texture.hpp"
#include "graphic/helper_graphic_pipeline.hpp"
#include "graphic/system_graphic_pipeline.hpp"
#include "graphic/system_particule.hpp"
#include "audio/system_audio.hpp"
#include "system_postprocessing.hpp"

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "physic/system_physic.hpp"
#include "thread"

namespace Game
{
  namespace
  {
    GameContext *game_context(Registry &registry)
    {
      if (auto *ptr = registry.try_ctx<GameContext>(); ptr)
      {
        return ptr;
      }
      std::cout << "game context not initialized !";
      exit(1);
    }
    void init_game(Registry &registry, float w, float h)
    {
      registry.set<GameContext>();
      auto world = registry.create();
      registry.emplace<entt::tag<Game::WORLD_TAG>>(world);
      registry.emplace<World>(world, 0.0f, glm::vec2(w, h));
    }
    void game_update(Registry &registry, float dt)
    {
      auto context = game_context(registry);
      context->delatTime = dt;
      auto worldHandle = world(registry);
      worldHandle.patch<World>([dt](World &world) {
        world.deltaTime = dt;
      });
    }
  } // namespace
  entt::basic_handle<entt::entity> world(Registry &registry)
  {
    auto worlds = registry.view<entt::tag<Game::WORLD_TAG>>();
    entt::entity entity = entt::null;
    if (worlds.size() > 0)
    {
      return entt::basic_handle(registry, worlds[0]);
    }
    std::cout << "No world initialized \n";
    return entt::basic_handle(registry, entity);
  }
} // namespace Game

void Game::processInput(Registry &registry, int action)
{
  auto context = game_context(registry);
  Gameplay::processInput(registry,
                         context->delatTime, // delta time
                         action              // action
  );
}
void Game::render(Registry &registry)
{
  Graphic::render(registry);
  Graphic::Particule::render(registry);
  Graphic::PostProcessing::render(registry);
}
void Game::update(Registry &registry, float dt, float time)
{
  game_update(registry, dt);
  Audio::update(registry, dt);
  Gameplay::update(registry, dt);
  Physic::update(registry, dt);
  Graphic::PostProcessing::update(registry, time);
  Graphic::update(registry, dt);
  Graphic::Particule::update(registry, dt);
}
void Game::init(Registry &registry, float w, float h)
{
  init_game(registry, w, h);
  Audio::init(registry);

  Audio::load_sound("background"_hs, "./assets/breakout.ogg");

  Physic::init(registry);
  Graphic::init(registry);
  Graphic::PostProcessing::init(registry,
                                static_cast<float>(w),
                                static_cast<float>(h),
                                "./shader/framebuffer.vect",
                                "./shader/framebuffer.frag");
  auto atlas_image =
      Graphic::load_image("atlas"_hs, "./texture/atlas.png", false);
  Graphic::load_texture("atlas"_hs, atlas_image);

  Graphic::Particule::init(registry);
  Gameplay::init(registry, w, h);

  auto main_vs_source = Graphic::load_shader_source(
      "main_vs"_hs, "./shader/main.vect", Graphic::VertexShader);
  auto main_fs_source = Graphic::load_shader_source(
      "main_fs"_hs, "./shader/main.frag", Graphic::FragmentShader);

  auto main_vs = Graphic::load_shader("main_vs"_hs, main_vs_source);
  auto main_fs = Graphic::load_shader("main_fs"_hs, main_fs_source);

  auto main_shader_r = Graphic::load_shader_program(
      "main"_hs, std::vector<Graphic::Shader>{main_vs, main_fs});

  Graphic::add_projection_matrix(glm::ortho(
      0.0f, static_cast<float>(w), static_cast<float>(h), 0.0f, -1.0f, 1.0f));
}
