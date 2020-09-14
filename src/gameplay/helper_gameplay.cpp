#include "gameplay/helper_gameplay.hpp"
#include "gameplay/component_gameplay.hpp"
#include "graphic/system_graphic_pipeline.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "game.h"
#include <GLFW/glfw3.h>
#include "physic/component_physic.hpp"
#include <glm/gtc/matrix_transform.hpp>
namespace Gameplay
{

  namespace
  {

    entt::registry gamePlayRegistry;
    Direction vector_direction(glm::vec2 target)
    {
      glm::vec2 compass[] = {
          glm::vec2(0.0f, 1.0f),  // up
          glm::vec2(1.0f, 0.0f),  // right
          glm::vec2(0.0f, -1.0f), // down
          glm::vec2(-1.0f, 0.0f)  // left
      };
      float max = 0.0f;
      unsigned int best_match = -1;
      for (unsigned int i = 0; i < 4; i++)
      {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
          max = dot_product;
          best_match = i;
        }
      }
      return (Direction)best_match;
    }
    void handle_error_context()
    {
      std::cout << "gameplay context not initialized !";
      exit(1);
    }
    entt::basic_handle<entt::entity> player(entt::registry &registry)
    {
      auto players = registry.view<entt::tag<Gameplay::player_tag>>();
      if (players.size() > 0)
      {
        return entt::basic_handle(registry, players[0]);
      }
      std::cout << "No player initialized \n";
      return entt::basic_handle(registry, static_cast<entt::entity>(entt::null));
    }
    entt::basic_handle<entt::entity> ball(entt::registry &registry)
    {
      auto balls = registry.view<Ball>();
      if (balls.size() > 0)
      {
        return entt::basic_handle(registry, balls[0]);
      }
      std::cout << "No ball initialized \n";
      return entt::basic_handle(registry, static_cast<entt::entity>(entt::null));
    }
    void render_ball(entt::registry &registry, unsigned int w, unsigned int h)
    {
      if (auto *ptr = gamePlayRegistry.try_ctx<GameplayContext>(); ptr)
      {
        const float ballRadius = 12.5f;
        auto [playerPosition, transform, movable] = player(registry).get<Graphic::Position, Graphic::Transform, Movable>();
        const glm::vec2 size(ballRadius * 2, ballRadius * 2);

        glm::vec3 pos = playerPosition.value + glm::vec3(transform.size.x / 2.0f - ballRadius,
                                                         -ballRadius * 2.0f, 0.0f);

        auto ball = registry.create();
        registry.emplace<Ball>(ball);
        registry.emplace<Graphic::Position>(ball, pos);
        registry.emplace<Graphic::Transform>(ball, 0.0f, size);
        registry.emplace<Graphic::RenderSprite>(ball, ptr->ball_sprite,
                                                ptr->player_render_group,
                                                glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        registry.emplace<Velocity>(ball, glm::vec3(100.0f, -350.0f, 0.0f));
        registry.emplace<Movable>(ball, movable.speed);
        registry.emplace<Physic::RigidBody>(ball, 1 << 0, 1 << 1);
        registry.emplace<Physic::SphereCollider>(ball, ballRadius);
        return;
      }
      handle_error_context();
    }
    void render_player(entt::registry &registry, unsigned int w, unsigned int h)
    {
      if (auto *ptr = gamePlayRegistry.try_ctx<GameplayContext>(); ptr)
      {
        // init player;
        const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
        glm::vec3 playerPos = glm::vec3(w / 2.0f - PLAYER_SIZE.x / 2.0f,
                                        h - PLAYER_SIZE.y - 10, 1.0f);

        auto player = registry.create();
        registry.emplace<entt::tag<Gameplay::player_tag>>(player);
        registry.emplace<Graphic::Position>(player, playerPos);
        registry.emplace<Graphic::Transform>(player, 0.0f, PLAYER_SIZE);
        registry.emplace<Graphic::RenderSprite>(player,
                                                ptr->player_sprite,
                                                ptr->player_render_group,
                                                glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        registry.emplace<Movable>(player, glm::vec3(500, 0, 0));
        registry.emplace<Physic::RigidBody>(player, 1 << 2, 1 << 0);
        registry.emplace<Physic::AABB>(player, playerPos + glm::vec3(PLAYER_SIZE, 0), playerPos);
        return;
      }
      handle_error_context();
    }
    void render_backgroud(entt::registry &registry, unsigned int w,
                          unsigned int h)
    {
      if (auto *ptr = gamePlayRegistry.try_ctx<GameplayContext>(); ptr)
      {
        auto background = registry.create();
        registry.emplace<Graphic::Position>(background,
                                            glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
        registry.emplace<Graphic::Transform>(background, 0.0f, glm::vec2(w, h));
        registry.emplace<Graphic::RenderSprite>(background, ptr->backgroud_sprite,
                                                ptr->backgroud_render_group,
                                                glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        return;
      }
      handle_error_context();
    }
    void on_ball_player_collision(entt::registry &registry, entt::entity a, entt::entity b)
    {
      if (registry.has<Ball>(a) && (registry.has<entt::tag<player_tag>>(b)))
      {
        auto aOtherPosition = registry.get<Graphic::Position>(b);
        auto ballPosition = registry.get<Graphic::Position>(a);
        auto balLTransform = registry.get<Graphic::Transform>(a);
        auto otherTransform = registry.get<Graphic::Transform>(b);
        auto velocity = registry.get<Velocity>(a);
        auto sphere = registry.get<Physic::SphereCollider>(a);

        auto ballCenter = ballPosition.value + (glm::vec3(balLTransform.size, 1.0f) / glm::vec3(2));
        auto aOtherCenter = aOtherPosition.value + (glm::vec3(otherTransform.size, 1.0f) / glm::vec3(2));
        float distance = (ballPosition.value.x + sphere.radius) - aOtherCenter.x;
        float percentage = distance / (otherTransform.size.x / 2.0f);

        float strength = 2.0f;
        glm::vec2 oldVelocity = velocity.value;
        glm::vec3 velocityInitial(100.0f, -350.0f, 0.0f);
        velocity.value.x = velocityInitial.x * percentage * strength;
        velocity.value.y = -velocity.value.y;
        velocity.value = glm::normalize(velocity.value) * glm::length(oldVelocity);
        registry.patch<Velocity>(a, [velocity](Velocity &v) {
          v.value = velocity.value;
        });
      }
    }
    void on_ball_brick_collision(entt::registry &registry, entt::entity a, entt::entity b)
    {
      if (registry.has<Ball>(a) && registry.has<Brick>(b))
      {
        auto brick = registry.get<Brick>(b);
        if (brick.type != BrickType::SOLID)
        {
          registry.patch<Brick>(b, [](Brick &brick) {
            brick.destroyed = true;
          });
          registry.emplace<Graphic::Destroy>(b);
          registry.remove_if_exists<Physic::RigidBody>(b);
        }
      }
      if (registry.has<Ball>(a) && (registry.has<Brick>(b)))
      {
        auto aOtherPosition = registry.get<Graphic::Position>(b);
        auto ballPosition = registry.get<Graphic::Position>(a);
        auto balLTransform = registry.get<Graphic::Transform>(a);
        auto otherTransform = registry.get<Graphic::Transform>(b);
        auto velocity = registry.get<Velocity>(a);
        auto sphere = registry.get<Physic::SphereCollider>(a);
        auto ballCenter = ballPosition.value + (glm::vec3(balLTransform.size, 1.0f) / glm::vec3(2));
        auto aOtherCenter = aOtherPosition.value + (glm::vec3(otherTransform.size, 1.0f) / glm::vec3(2));
        glm::vec3 centers_distance = glm::normalize(aOtherCenter - ballCenter);
        Direction dir = vector_direction(glm::vec2(centers_distance));
        if (dir == LEFT || dir == RIGHT)
        {
          registry.patch<Velocity>(a, [velocity](Velocity &v) {
            v.value.x = -v.value.x;
          });
          float penetrationValue = sphere.radius - std::abs(centers_distance.x);
          if (dir == LEFT)
          {
            registry.patch<Graphic::Position>(a, [penetrationValue](Graphic::Position &p) {
              p.value.x += penetrationValue;
            });
          }
          else
          {
            registry.patch<Graphic::Position>(a, [penetrationValue](Graphic::Position &p) {
              p.value.x -= penetrationValue;
            });
          }
        }
        else
        {
          registry.patch<Velocity>(a, [velocity](Velocity &v) {
            v.value.y = -v.value.y;
          });
          float penetrationValue = sphere.radius - std::abs(centers_distance.y);
          if (dir == UP)
          {
            registry.patch<Graphic::Position>(a, [penetrationValue](Graphic::Position &p) {
              p.value.y -= penetrationValue;
            });
          }
          else
          {
            registry.patch<Graphic::Position>(a, [penetrationValue](Graphic::Position &p) {
              p.value.y += penetrationValue;
            });
          }
        }
      }
    }
    void on_collision(entt::registry &registry)
    {
      auto view = registry.view<Physic::Collision>();
      if (view.size() > 0)
      {
        for (auto entity : view)
        {
          auto collision = view.get<Physic::Collision>(entity);
          on_ball_brick_collision(registry, collision.a, collision.b);
          on_ball_brick_collision(registry, collision.b, collision.a);
          on_ball_player_collision(registry, collision.a, collision.b);
          on_ball_player_collision(registry, collision.b, collision.a);
        }
      }
    }
    void ball_mouvement(entt::registry &registry)
    {
      auto world = Game::world(registry).get<Game::World>();
      auto balls = registry.view<Ball, Velocity, Graphic::Position, Graphic::Transform>(entt::exclude<Movable>);
      for (auto entity : balls)
      {
        auto transform = registry.get<Graphic::Transform>(entity);
        registry.patch<Velocity>(entity, [&registry, entity, world, transform](Velocity &velocity) {
          registry.patch<Graphic::Position>(entity, [&velocity, world, transform](Graphic::Position &position) {
            position.value += velocity.value * world.deltaTime;
            if (position.value.x <= 0.0f)
            {
              velocity.value.x = -velocity.value.x;
              position.value.x = 0.0f;
            }
            else if (position.value.x + transform.size.x >= world.viewport.x)
            {
              velocity.value.x = -velocity.value.x;
              position.value.x = world.viewport.x - transform.size.x;
            }
            if (position.value.y <= 0.0f)
            {
              velocity.value.y = -velocity.value.y;
              position.value.y = 0.0f;
            }
            /*  else if (position.value.y + transform.size.y >= world.viewport.y)
            {
              velocity.value.y = -velocity.value.y;
              position.value.y = world.viewport.y - transform.size.y;
            } */
          });
        });
      }
    }
  } // namespace
  std::shared_ptr<Level> level_loader::load(const std::string &filePath,
                                            unsigned int lvlWidth,
                                            unsigned int lvlHeight) const
  {
    std::shared_ptr<Level> level = std::shared_ptr<Level>(new Level());
    std::ifstream file(filePath);
    std::string line;
    std::vector<BrickType> types = {BrickType::BREAKABLE, BrickType::SOLID,
                                    BrickType::BREAKABLE, BrickType::BREAKABLE,
                                    BrickType::BREAKABLE, BrickType::BREAKABLE};
    std::vector<glm::vec4> colors = {
        glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.8f, 0.8f, 0.7f, 1.0f),
        glm::vec4(0.2f, 0.6f, 1.0f, 1.0f), glm::vec4(0.0f, 0.7f, 0.0f, 1.0f),
        glm::vec4(0.8f, 0.8f, 0.4f, 1.0f), glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)};
    while (std::getline(file, line))
    {
      std::stringstream lineStream(line);
      int value;
      level->rows = 0;
      while (lineStream >> value)
      {
        BrickType brick;
        glm::vec4 color;
        if (value < types.size())
        {
          brick = types[value];
          color = colors[value];
        }
        if (value != 0)
        {
          level->colors.push_back(color);
          level->positions.push_back(glm::vec2(level->rows, level->cols));
          level->bricks.push_back(brick);
        }
        level->rows++;
      }
      level->cols++;
    }
    level->unit_width = lvlWidth / static_cast<float>(level->rows);
    level->unit_height = lvlHeight / static_cast<float>(level->cols);
    for (auto &position : level->positions)
    {
      position.x *= level->unit_width;
      position.y *= level->unit_height;
    }
    return level;
  }
  void init(entt::registry &registry, unsigned int w, unsigned int h)
  {
    auto &context = gamePlayRegistry.set<GameplayContext>();

    auto atlas_image =
        Graphic::load_image("atlas"_hs, "./texture/atlas.png", false);
    auto backgroud_image =
        Graphic::load_image("backgroud"_hs, "./texture/background.jpg", false);

    auto &atlas = Graphic::load_texture("atlas"_hs, atlas_image);
    auto &background_texture =
        Graphic::load_texture("background"_hs, backgroud_image);

    context.backgroud_render_group =
        Graphic::create_render_group("main"_hs, "background"_hs);
    context.brick_render_group =
        Graphic::create_render_group("main"_hs, "atlas"_hs);
    context.player_render_group =
        Graphic::create_render_group("main"_hs, "atlas"_hs);

    context.brick_sprite_solid =
        Graphic::Sprite({atlas, glm::vec4(0, 512, 128, 128)});
    context.brick_sprite_breakable =
        Graphic::Sprite({atlas, glm::vec4(128, 512, 128, 128)});
    context.player_sprite = Graphic::Sprite({atlas, glm::vec4(0, 640, 512, 128)});
    context.ball_sprite = Graphic::Sprite({atlas, glm::vec4(0, 0, 512, 512)});

    context.backgroud_sprite = Graphic::Sprite(
        {background_texture, glm::vec4(0, 0, background_texture.get().width,
                                       background_texture.get().height)});

    Gameplay::load_level("1"_hs, "./assets/one.txt", w, h / 2);
    Gameplay::load_level("2"_hs, "./assets/two.txt", w, h / 2);
    Gameplay::load_level("3"_hs, "./assets/three.txt", w, h / 2);
    Gameplay::load_level("4"_hs, "./assets/four.txt", w, h / 2);

    Gameplay::render_backgroud(registry, w, h);
    Gameplay::render_level(registry, "4"_hs);
    Gameplay::render_player(registry, w, h);
    Gameplay::render_ball(registry, w, h);
  }
  void update(entt::registry &registry)
  {
    on_collision(registry);
    ball_mouvement(registry);
  }
  entt::resource_handle<Level> load_level(entt::hashed_string &levelId,
                                          const std::string &path,
                                          unsigned int lvlWidth,
                                          unsigned int lvlHeight)
  {
    if (auto *ptr = gamePlayRegistry.try_ctx<GameplayContext>(); ptr)
    {
      auto level =
          ptr->level_cache.load<level_loader>(levelId, path, lvlWidth, lvlHeight);
      return level;
    }
    handle_error_context();
  }
  void render_level(entt::registry &registry, entt::hashed_string &levelId)
  {
    if (auto *ptr = gamePlayRegistry.try_ctx<GameplayContext>(); ptr)
    {
      Graphic::clear_render_group(ptr->brick_render_group);
      auto &level = ptr->level_cache.handle(levelId).get();
      for (int i = 0; i < level.bricks.size(); i++)
      {
        auto sprite = (level.bricks[i] == Gameplay::BrickType::BREAKABLE)
                          ? ptr->brick_sprite_breakable
                          : ptr->brick_sprite_solid;
        auto brick = registry.create();
        auto pos = glm::vec3(level.positions[i], 0.0f);
        auto size = glm::vec2(level.unit_width, level.unit_height);
        registry.emplace<Graphic::Position>(brick,
                                            pos);
        registry.emplace<Graphic::Transform>(
            brick, 0.0f, size);
        registry.emplace<Graphic::RenderSprite>(
            brick, sprite, ptr->brick_render_group, level.colors[i]);

        registry.emplace<Physic::RigidBody>(brick, 1 << 1, 1 << 0);
        registry.emplace<Physic::AABB>(brick, pos + glm::vec3(size, 0), pos);
        registry.emplace<Brick>(brick, level.bricks[i], false);
      }
      return;
    }
    handle_error_context();
  }
  void processInput(Registry &registry, float dt, int action)
  {
    auto players = registry.view<Movable,
                                 Graphic::Position, Graphic::Transform>();
    auto world = Game::world(registry).get<Game::World>();
    for (auto player : players)
    {
      auto velocity = players.get<Movable>(player);
      auto transform = players.get<Graphic::Transform>(player);
      if (action == GLFW_KEY_LEFT)
      {
        registry.patch<Graphic::Position>(
            player, [dt, velocity, world](Graphic::Position &position) {
              if (position.value.x >= 0.0f)
              {
                position.value.x -= velocity.speed.x * world.deltaTime;
              }
            });
      }
      if (action == GLFW_KEY_RIGHT)
      {
        registry.patch<Graphic::Position>(
            player, [dt, velocity, world, transform](Graphic::Position &position) {
              if (position.value.x <= world.viewport.x - transform.size.x)
                position.value.x += velocity.speed.x * world.deltaTime;
            });
      }
    }
    if (action == GLFW_KEY_SPACE)
    {
      auto ballHandle = ball(registry);
      ballHandle.remove_if_exists<Movable>();
    }
  }
} // namespace Gameplay