#pragma once
#include "graphic/component_render.hpp"
#include "ecs.h"
#include "gameplay/component_gameplay.hpp"
#include "graphic/system_graphic_pipeline.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

namespace Gameplay
{

  struct BreakoutGame
  {
    entt::hashed_string currentLevel;
  };
  enum GameState
  {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
  };
  namespace
  {

    enum Direction
    {
      UP,
      RIGHT,
      DOWN,
      LEFT
    };
    struct GameplayContext
    {
      GameState gameState;
      entt::resource_cache<Level> level_cache;
      Graphic::Sprite brick_sprite_solid;
      Graphic::Sprite brick_sprite_breakable;
      Graphic::Sprite player_sprite;
      Graphic::Sprite ball_sprite;
      Graphic::Sprite backgroud_sprite;
      Graphic::Sprite powers_up[6];
      Graphic::RenderGroupHandle brick_render_group;
      Graphic::RenderGroupHandle player_render_group;
      Graphic::RenderGroupHandle backgroud_render_group;
    };
  } // namespace
  struct level_loader final : entt::resource_loader<level_loader, Level>
  {
    std::shared_ptr<Level> load(const std::string &filePath,
                                unsigned int lvlWidth,
                                unsigned int lvlHeight) const;
  };
  void init(entt::registry &registry, unsigned int w, unsigned int h);
  entt::resource_handle<Level> load_level(entt::hashed_string &levelId,
                                          const std::string &path,
                                          unsigned int lvlWidth,
                                          unsigned int lvlHeight);
  void render_level(entt::registry &registry, entt::hashed_string &levelId);
  void process_input(Registry &registry, float dt, int action);
  void update(Registry &registry, float dt);
} // namespace Gameplay