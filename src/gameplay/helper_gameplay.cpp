
#pragma once
#include "gameplay/helper_gameplay.hpp"
#include "graphic/system_graphic_pipeline.hpp"
#include "physic/helper_physic.hpp"
#include "audio/component_audio.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "game.h"
#include <GLFW/glfw3.h>
#include "physic/component_physic.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "system_postprocessing.hpp"
#include <functional>
#include "audio/system_audio.hpp"

namespace Gameplay
{

  auto nBottomMiddle = glm::vec3(0.f, 1.f, 0.f);
  auto nBottomLeft = glm::normalize(glm::vec3(0.f, 1.f, 0.f));
  auto nBottomRight = glm::normalize(glm::vec3(0.f, 1.f, 0.f));
  auto nLeftMiddle = glm::normalize(glm::vec3(1.f, 0.9f, 0.f));
  auto nRightMiddle = glm::normalize(glm::vec3(-1.f, 0.9f, 0.f));
  auto nTopLeft = glm::normalize(glm::vec3(-0.f, -1.f, 0.f));
  auto nTopRight = glm::normalize(glm::vec3(0.f, -1.f, 0.f));
  auto nTopMiddle = glm::vec3(0.f, -1.f, 0.f);
  constexpr glm::vec3 VELOCITY_INITIAL(50.0f, -350.0f, 0.0f);
  constexpr glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
  constexpr glm::vec3 PLAYER_SPEED(2500.0f, 0.0f, 0.0f);
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
      if (balls.size() == 1)
      {
        return entt::basic_handle(registry, balls[0]);
      }
      std::cout << "No ball initialized \n";
      return entt::basic_handle(registry, static_cast<entt::entity>(entt::null));
    }
    void render_powerup(entt::registry &registry, const Gameplay::PowerUp::Type type, const glm::vec4 &color, const glm::vec3 position)
    {
      if (auto *ptr = gamePlayRegistry.try_ctx<GameplayContext>(); ptr)
      {
        const glm::vec2 SIZE(60.0f, 20.0f);
        const glm::vec3 VELOCITY(0.0f, 150.0f, 0.0f);
        auto powerup = registry.create();
        registry.emplace<Graphic::Position>(powerup, position);
        registry.emplace<Graphic::Transform>(powerup,
                                             0.0f, // rotation
                                             SIZE  // size
        );
        Graphic::Sprite sprite;
        switch (type)
        {
        case Gameplay::PowerUp::Type::Sticky:
          sprite = ptr->powers_up[5];
          break;
        case Gameplay::PowerUp::Type::Speed:
          sprite = ptr->powers_up[4];
          break;
        case Gameplay::PowerUp::Type::PassThrough:
          sprite = ptr->powers_up[3];
          break;
        case Gameplay::PowerUp::Type::PadSizeIncrease:
          sprite = ptr->powers_up[2];
          break;
        case Gameplay::PowerUp::Type::Confuse:
          sprite = ptr->powers_up[1];
          break;
        case Gameplay::PowerUp::Type::Chaos:
          sprite = ptr->powers_up[0];
          break;
        default:
          break;
        }
        registry.emplace<Graphic::RenderSprite>(powerup,
                                                sprite,                   // Sprite
                                                ptr->player_render_group, // render groupe
                                                color                     // color
        );
        registry.emplace<PowerUp>(powerup, type);
        registry.emplace<Velocity>(powerup, VELOCITY);
        registry.emplace<Physic::RigidBody>(powerup, 1 << 0, 1 << 1);
        registry.emplace<Physic::AABB>(powerup, position + glm::vec3(SIZE, 0), position);
        return;
      }
      handle_error_context();
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

        registry.emplace<Ball>(ball,
                               false, // sticky
                               false  // passthrough
        );
        registry.emplace<Graphic::Position>(ball, pos);
        registry.emplace<Graphic::Transform>(ball,
                                             0.0f, // rotate,
                                             size  // size
        );
        registry.emplace<Graphic::RenderSprite>(ball,
                                                ptr->ball_sprite,                 //sprite
                                                ptr->player_render_group,         //render groupe
                                                glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) // color
        );
        registry.emplace<Velocity>(ball, VELOCITY_INITIAL);
        registry.emplace<Movable>(ball, movable.speed);
        registry.emplace<Physic::RigidBody>(ball,
                                            1 << 0, // category
                                            1 << 1  // collade with
        );
        registry.emplace<Physic::SphereCollider>(ball, ballRadius);
        auto particles = registry.view<Graphic::Particle>();
        for (auto [entity, particule] : particles.each())
        {
          registry.emplace<Graphic::Target>(entity, ball);
        }
        return;
      }
      handle_error_context();
    }
    void render_player(entt::registry &registry, unsigned int w, unsigned int h)
    {
      if (auto *ptr = gamePlayRegistry.try_ctx<GameplayContext>(); ptr)
      {
        glm::vec3 playerPos = glm::vec3(w / 2.0f - PLAYER_SIZE.x / 2.0f,
                                        h - PLAYER_SIZE.y - 10, 1.0f);

        auto player = registry.create();
        registry.emplace<entt::tag<Gameplay::player_tag>>(player);
        registry.emplace<Graphic::Position>(player, playerPos);
        registry.emplace<Graphic::Transform>(player,
                                             0.0f,       // rotation
                                             PLAYER_SIZE // size
        );
        registry.emplace<Graphic::RenderSprite>(player,
                                                ptr->player_sprite,               // rotate
                                                ptr->player_render_group,         // render groupe
                                                glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) // color
        );
        registry.emplace<Movable>(player, PLAYER_SPEED);
        registry.emplace<Physic::RigidBody>(player, 1 << 2 /** category **/, 1 << 0 /** collade with **/);
        registry.emplace<Physic::AABB>(player,
                                       playerPos + glm::vec3(PLAYER_SIZE.x, 0.5f, 0.f) // upperbound
                                       ,
                                       playerPos // lowerbound
        );
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
                                            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        registry.emplace<Graphic::Transform>(background,
                                             0.0f,           // rotate
                                             glm::vec2(w, h) // size
        );
        registry.emplace<Graphic::RenderSprite>(background,
                                                ptr->backgroud_sprite,            // sprite
                                                ptr->backgroud_render_group,      // render group
                                                glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) // colro
        );
        return;
      }
      handle_error_context();
    }
    Physic::AABB split(Physic::AABB aabb, float lx, float ly, float ux, float uy)
    {
      auto origin = aabb.lowerBound;
      auto h = aabb.upperBound.y - aabb.lowerBound.y;
      auto w = aabb.upperBound.x - aabb.lowerBound.x;

      auto split = Physic::AABB();
      split.lowerBound.x = origin.x + (w * lx);
      split.lowerBound.y = origin.y + (h * ly);
      split.upperBound.x = origin.x + (w * ux);
      split.upperBound.y = origin.y + (h * uy);

      return split;
    }
    bool should_spawn(unsigned int chance)
    {
      unsigned int random = rand() % chance;
      return random == 0;
    }
    void spawn_powerup(entt::registry &registry, const glm::vec3 &position)
    {
      const int POSITIVE_EFFECT_SPAWN = 50;
      const int NEGATIVE_EFFECT_SPAWN = 40;
      if (should_spawn(POSITIVE_EFFECT_SPAWN))
      {
        render_powerup(registry, PowerUp::Type::Speed, glm::vec4(0.5f, 0.5f, 1.0f, 1.0f), position);
      }

      if (should_spawn(POSITIVE_EFFECT_SPAWN))
      {
        render_powerup(registry, PowerUp::Type::Sticky, glm::vec4(1.0f, 0.5f, 1.0f, 1.0f), position);
      }

      if (should_spawn(POSITIVE_EFFECT_SPAWN))
      {
        render_powerup(registry, PowerUp::Type::PassThrough, glm::vec4(0.5f, 1.0f, 0.5f, 1.0f), position);
      }

      if (should_spawn(POSITIVE_EFFECT_SPAWN))
      {
        render_powerup(registry, PowerUp::Type::PadSizeIncrease, glm::vec4(1.0f, 0.6f, 0.4f, 1.0f), position);
      }

      if (should_spawn(NEGATIVE_EFFECT_SPAWN)) // negative powerups should spawn more often
        render_powerup(registry, PowerUp::Type::Confuse, glm::vec4(1.0f, 0.3f, 0.3f, 1.0f), position);

      if (should_spawn(NEGATIVE_EFFECT_SPAWN))
        render_powerup(registry, PowerUp::Type::Chaos, glm::vec4(0.9f, 0.25f, 0.25f, 1.0f), position);
    }
    void reset_ball(entt::registry &registry)
    {
      auto playerHandle = player(registry);
      auto ballHandle = ball(registry);
      auto [playerPosition, transform, movable] =
          playerHandle.get<Graphic::Position, Graphic::Transform, Movable>();
      auto collision = ballHandle.get<Physic::SphereCollider>();

      // reset ball
      glm::vec3 ballPosition = playerPosition.value +
                               glm::vec3(transform.size.x / 2.0f - collision.radius,
                                         -collision.radius * 2.0f, 0.0f);
      ballHandle.replace<Graphic::Position>(ballPosition);
      ballHandle.replace<Velocity>(VELOCITY_INITIAL);
      ballHandle.emplace<Movable>(movable.speed);
    }
    void reset_player(entt::registry &registry)
    {
      auto playerHandle = player(registry);
      auto ballHandle = ball(registry);
      auto world = Game::world(registry).get<Game::World>();
      auto [playerPosition, transform, movable] =
          playerHandle.get<Graphic::Position, Graphic::Transform, Movable>();
      auto collision = ballHandle.get<Physic::SphereCollider>();

      // reset player
      glm::vec3 playerPos =
          glm::vec3(world.viewport.x / 2.0f - PLAYER_SIZE.x / 2.0f,
                    world.viewport.y - PLAYER_SIZE.y - 10, 1.0f);

      playerHandle.replace<Graphic::Position>(playerPos);

      // reset ball
      reset_ball(registry);
    }
    glm::vec3 paddle_collision(Physic::AABB brick, glm::vec3 ballPosition, float radius)
    {
      auto left = split(brick, 0, 0, 1.f / 4.f, 1.f / 4.f);
      auto middle = split(brick, 1.f / 4.f, 0, 3.f / 4.f, 1.f / 4.f);
      auto right = split(brick, 3.f / 4.f, 0, 1.f, 1.f);
      auto bottom = split(brick, 0, 1.f / 4.f, 1.f, 1.f);

      if (Physic::intersect(left, ballPosition, radius))
      {
        return glm::vec3(-1.f, -1.f, 0.f);
      }
      if (Physic::intersect(right, ballPosition, radius))
      {
        return glm::vec3(1.f, -1.f, 0.f);
      }
      if (Physic::intersect(middle, ballPosition, radius))
      {
        return glm::vec3(0.f, -1.f, 0.f);
      }
      if (Physic::intersect(bottom, ballPosition, radius))
      {
        return glm::vec3(0.f, 1.f, 0.f);
      }

      return glm::vec3(1.f, 1.f, 0.f);
    }
    float hit_factor(glm::vec3 paddlePosition, glm::vec3 ballPosition, float paddleWidth)
    {
      return (ballPosition.x - paddlePosition.x) / paddleWidth;
    }
    void on_player_powerup_collision(entt::registry &registry, entt::entity a, entt::entity b)
    {
      if (registry.all_of<entt::tag<player_tag>>(a) && registry.all_of<PowerUp>(b))
      {
        const auto powerup = registry.get<PowerUp>(b);
        if (registry.all_of<Graphic::Destroy>(b))
        {
          /* registry.destroy(b); */
        }
        else
        {
          registry.emplace<Graphic::Destroy>(b);
          registry.remove<Physic::RigidBody>(b);
          registry.emplace<Duration>(b, 10.f);
          registry.emplace<Graphic::Target>(b, a);
        }
      }
    }
    void on_ball_player_collision(entt::registry &registry, entt::entity a, entt::entity b)
    {
      if (registry.all_of<Ball>(a) && registry.all_of<entt::tag<player_tag>>(b))
      {
        auto position = registry.get<Graphic::Position>(a);
        auto ball = registry.get<Ball>(a);
        auto paddleTransform = registry.get<Graphic::Transform>(b);
        auto aabb = registry.get<Physic::AABB>(b);
        auto sphere = registry.get<Physic::SphereCollider>(a);
        auto paddlePosition = registry.get<Graphic::Position>(b);
        auto velocity = registry.get<Velocity>(a);
        auto force = glm::length(velocity.value);
        auto factor = hit_factor(paddlePosition.value + (glm::vec3(paddleTransform.size, 0.f) / 2.f),
                                 position.value + (sphere.radius / 2.f),
                                 (paddleTransform.size.x) / 2.f);
        std::cout << "hit factor:" << factor << "\n";
        std::cout << "paddle position(" << (paddlePosition.value + (glm::vec3(paddleTransform.size, 0.f) / 2.f)).x << ") ";
        std::cout << "balle position(" << (position.value - (sphere.radius)).x << ") ";
        std::cout << "paddle size(" << paddleTransform.size.x - sphere.radius << ") \n";
        registry.patch<Velocity>(a, [force, factor](Velocity &v) {
          v.value = force * glm::normalize(glm::vec3(factor, -1.f, 0.f));
        });
        if (ball.sticky)
        {
          auto collision = registry.get<Physic::SphereCollider>(a);
          auto movable = registry.get<Movable>(b);
          // reset ball
          glm::vec3 ballPosition = position.value +
                                   glm::vec3(-collision.radius,
                                             -collision.radius * 2.0f, 0.0f);
          registry.replace<Graphic::Position>(a, ballPosition);
          registry.emplace_or_replace<Movable>(a, movable.speed);
          return;
        }
        // play sound
        auto soundToPlay = Audio::Play2D("bleep"_hs, false, 2500);
        if (registry.all_of<Audio::Play2D>(a) == false)
        {
          registry.emplace<Audio::Play2D>(a, soundToPlay);
        }
        else
        {
          registry.replace<Audio::Play2D>(a, soundToPlay);
        }
      }
    }

    glm::vec3 brick_collision(Physic::AABB brick, glm::vec3 ballPosition, float radius)
    {

      auto topLeftCorner = split(brick, 0, 0, 1.f / 5.f, 1.f / 4.f);
      auto topMiddle = split(brick, 1.f / 5.f, 0, 4.f / 5.f, 1.f / 4.f);
      auto topRightCorner = split(brick, 4.f / 5.f, 0, 1.f, 1.f / 4.f);
      auto rightMiddle = split(brick, 4.f / 5.f, 1.f / 4.f, 1.f, 3.f / 4.f);
      auto bottomRight = split(brick, 5.f / 5.f, 3.f / 4.f, 1.f, 1.f);
      auto bottomMiddle = split(brick, 1.f / 5.f, 3.f / 4.f, 4.f / 5.f, 1.f);
      auto bottomLeft = split(brick, 0.f, 3.f / 4.f, 1.f / 5.f, 1.f);
      auto leftMiddle = split(brick, 0.f, 1.f / 4.f, 1.f / 5.f, 3.f / 4.f);

      if (Physic::intersect(bottomLeft, ballPosition, radius))
      {
        return nBottomLeft;
      }
      if (Physic::intersect(bottomRight, ballPosition, radius))
      {
        return nBottomRight;
      }
      if (Physic::intersect(topRightCorner, ballPosition, radius))
      {
        return nTopRight;
      }
      if (Physic::intersect(topLeftCorner, ballPosition, radius))
      {
        return nTopLeft;
      }
      if (Physic::intersect(topMiddle, ballPosition, radius))
      {
        return nTopMiddle;
      }
      if (Physic::intersect(bottomMiddle, ballPosition, radius))
      {
        return nBottomMiddle;
      }
      if (Physic::intersect(rightMiddle, ballPosition, radius))
      {
        return nRightMiddle;
      }
      if (Physic::intersect(leftMiddle, ballPosition, radius))
      {
        return nLeftMiddle;
      }
      return glm::vec3(0.f, 1.0f, 0.f);
    }
    void on_ball_brick_collision(entt::registry &registry, entt::entity a, entt::entity b)
    {
      auto isBallBrickCollision = registry.all_of<Ball>(a) && registry.all_of<Brick, Graphic::Position>(b);
      if (isBallBrickCollision)
      {
        auto brick = registry.get<Brick>(b);
        auto aOtherPosition = registry.get<Graphic::Position>(b);
        auto otherTransform = registry.get<Graphic::Transform>(b);
        auto aOtherCenter = aOtherPosition.value + (glm::vec3(otherTransform.size, 1.0f) / glm::vec3(2));
        if (brick.type != BrickType::SOLID)
        {
          registry.patch<Brick>(b, [](Brick &brick) {
            brick.destroyed = true;
          });
          registry.emplace<Graphic::Destroy>(b);
          registry.remove_if_exists<Physic::RigidBody>(b);
          auto ball = registry.get<Ball>(a);
          spawn_powerup(registry, aOtherCenter);
          if (ball.passThrough)
          {
            return;
          }
        }
      }
      if (isBallBrickCollision)
      {
        auto aOtherAABB = registry.get<Physic::AABB>(b);
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
        glm::vec3 result = brick_collision(aOtherAABB, ballPosition.value, sphere.radius);
        registry.patch<Velocity>(a, [velocity, result, ballCenter, aOtherPosition](Velocity &v) {
          v.value = glm::reflect(velocity.value, result);
          if (result == nBottomLeft || result == nBottomMiddle || result == nBottomRight)
          {
            v.value.y = abs(v.value.y);
          }
          if (result == nTopMiddle)
          {
            v.value.y = -abs(v.value.y);
          }
        });
      }
      if (isBallBrickCollision)
      {
        auto viewShake = registry.view<Duration, entt::tag<"shake"_hs>>();
        auto brick = registry.get<Brick>(b);
        auto soundToPlayId = brick.type == BrickType::SOLID ? "solid"_hs : "not_solid"_hs;
        auto soundToPlay = Audio::Play2D(soundToPlayId, false, 3500);
        for (auto shake : viewShake)
        {
          auto &duration = viewShake.get<Duration>(shake);
          duration.value = 0.2f;
          if (registry.all_of<Audio::Play2D>(shake) == false)
          {
            registry.emplace<Audio::Play2D>(shake, soundToPlay);
          }
          else
          {
            registry.replace<Audio::Play2D>(shake, soundToPlay);
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
          on_player_powerup_collision(registry, collision.b, collision.a);
          on_player_powerup_collision(registry, collision.a, collision.b);
        }
      }
    }
    // namespace
    entt::basic_handle<entt::entity> game_state(entt::registry &registry)
    {
      auto games = registry.view<BreakoutGame>();
      if (games.size() > 0)
      {
        return entt::basic_handle(registry, games[0]);
      }
      std::cout << "No player initialized \n";
      return entt::basic_handle(registry, static_cast<entt::entity>(entt::null));
    }
    void reset_level(entt::registry &registry)
    {
      auto gameHandle = game_state(registry);
      render_level(registry, gameHandle.get<BreakoutGame>().currentLevel);
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
  void create_shake(entt::registry &registry)
  {
    auto shake = registry.create();
    registry.emplace<Duration>(shake, 0.f);
    registry.emplace<entt::tag<"shake"_hs>>(shake);
  }
  void init_postprocessing(entt::registry &registry)
  {
    // post processing effect
    create_shake(registry);
    float offset = 1.0f / 300.0f;
    float offsets[9][2] = {
        {-offset, offset},  // top-left
        {0.0f, offset},     // top-center
        {offset, offset},   // top-right
        {-offset, 0.0f},    // center-left
        {0.0f, 0.0f},       // center-center
        {offset, 0.0f},     // center - right
        {-offset, -offset}, // bottom-left
        {0.0f, -offset},    // bottom-center
        {offset, -offset}   // bottom-right
    };
    float kernel[3][9] = {
        {-1.f, -1.f, -1.f,
         -1.f, 8.f, -1.f,
         -1.f, -1.f, -1.f},
        {-1.f, -1.f, -1.f,
         -1.f, 8.f, -1.f,
         -1.f, -1.f, -1.f},
        {1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
         2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
         1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f}};

    Graphic::ShaderProgam shaders[3] = {};
    Graphic::PostProcessing::load_effect("invert"_hs, "./shader/framebuffer.vect", "./shader/invert.frag");
    shaders[0] = Graphic::get_shader_program("invert"_hs).get();
    Graphic::PostProcessing::load_effect("chaos"_hs, "./shader/chaos.vect", "./shader/chaos.frag");
    shaders[1] = Graphic::get_shader_program("chaos"_hs).get();
    Graphic::PostProcessing::load_effect("shake"_hs, "./shader/shake.vect", "./shader/shake.frag");
    shaders[2] = Graphic::get_shader_program("shake"_hs).get();
    for (int i = 0; i < std::size(shaders); i++)
    {
      glUseProgram(shaders[i]);
      glUniform2fv(glGetUniformLocation(shaders[i], "offsets"), 9, (float *)offsets);
      glUniform1fv(glGetUniformLocation(shaders[i], "kernel"), 9, kernel[i]);
      glUseProgram(0);
    }
  }
  void init_game_sound()
  {

    Audio::load_sound("background"_hs, "./assets/breakout.ogg");
    Audio::load_sound("bleep"_hs, "./assets/bleep.wav");
    Audio::load_sound("powerup"_hs, "./assets/powerup.wav");
    Audio::load_sound("solid"_hs, "./assets/solid.wav");
    Audio::load_sound("not_solid"_hs, "./assets/not_solid.ogg");
  }
  void init_game_shader(unsigned int w, unsigned int h)
  {
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
  void init(entt::registry &registry, unsigned int w, unsigned int h)
  {

    init_game_shader(w, h);
    init_game_sound();
    auto &context = gamePlayRegistry.set<GameplayContext>();

    auto backgroud_image =
        Graphic::load_image("backgroud"_hs, "./texture/background.jpg", false);

    auto &atlas = Graphic::get_texture("atlas"_hs).get();

    auto &background_texture =
        Graphic::load_texture("background"_hs, backgroud_image);

    context.backgroud_render_group =
        Graphic::create_render_group("main"_hs, "background"_hs);
    context.brick_render_group =
        Graphic::create_render_group("main"_hs, "atlas"_hs);
    context.player_render_group =
        Graphic::create_render_group("main"_hs, "atlas"_hs);

    context.brick_sprite_solid =
        Graphic::Sprite({atlas, glm::vec4(128, 512, 128, 128)});
    context.brick_sprite_breakable =
        Graphic::Sprite({atlas, glm::vec4(0, 512, 128, 128)});

    context.player_sprite = Graphic::Sprite({atlas, glm::vec4(0, 640, 512, 128)});

    context.ball_sprite = Graphic::Sprite({atlas, glm::vec4(0, 0, 512, 512)});

    // POWER UP
    for (int i = 0; i <= 5; i++)
    {
      context.powers_up[i] = Graphic::Sprite({atlas, glm::vec4(0, 1268 + (i * 128), 512, 128)});
    }
    context.backgroud_sprite = Graphic::Sprite(
        {background_texture, glm::vec4(0, 0, background_texture.get().width,
                                       background_texture.get().height)});

    // create game state
    auto gameEntity = registry.create();
    auto gameState = registry.emplace<BreakoutGame>(gameEntity, "1"_hs);
    registry.emplace<Audio::Play2D>(gameEntity, "background"_hs, true);
    load_level("1"_hs, "./assets/one.txt", w, h / 2);
    load_level("2"_hs, "./assets/two.txt", w, h / 2);
    load_level("3"_hs, "./assets/three.txt", w, h / 2);
    load_level("4"_hs, "./assets/four.txt", w, h / 2);

    render_backgroud(registry, w, h);
    render_level(registry, gameState.currentLevel);
    render_player(registry, w, h);
    render_ball(registry, w, h);

    init_postprocessing(registry);
  }
  void mouvement(entt::registry &registry, std::function<void(const entt::entity &)> callback)
  {
    auto world = Game::world(registry).get<Game::World>();
    auto fallings = registry.view<Velocity, Graphic::Position, Graphic::Transform>(entt::exclude<Movable, Graphic::Destroy>);

    auto count = 0;
    for (auto entity : fallings)
    {
      count += 1;
      auto transform = registry.get<Graphic::Transform>(entity);
      registry.patch<Velocity>(entity, [&registry, entity, world, transform, callback](Velocity &velocity) {
        registry.patch<Graphic::Position>(entity, [entity, &registry, &velocity, world, transform, callback](Graphic::Position &position) {
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
          else if (position.value.y + transform.size.y >= world.viewport.y + transform.size.y * 2)
          {
            callback(entity);
          }
        });
      });
    }
  }
  void update_shake(entt::registry &registry, float dt)
  {
    auto view = registry.view<entt::tag<"shake"_hs>, Duration>();
    for (auto entity : view)
    {
      auto duration = view.get<Duration>(entity);
      if (duration.value > 0.f)
      {
        Graphic::PostProcessing::active_effect("shake"_hs);
      }
      else
      {
        Graphic::PostProcessing::desactive_effect("shake"_hs);
      }
    }
  }
  void update_duration(entt::registry &registry, float dt)
  {
    auto view = registry.view<Duration>();
    for (auto entity : view)
    {
      auto &duration = view.get<Duration>(entity);
      if (duration.value > 0.0f)
      {
        duration.value -= dt;
        if (duration.value < 0.f)
        {
          duration.value = 0.f;
        }
      }
    }
  }
  const std::string debug_powerup(PowerUp powerup)
  {
    char *PowerUpTypes[] =
        {
            "Chaos",
            "Speed",
            "Sticky",
            "PassThrough",
            "PadSizeIncrease",
            "Confuse"};
    return PowerUpTypes[static_cast<int>(powerup.type)];
  }
  void active_sticky(entt::registry &registry)
  {
    auto ballHandle = ball(registry);
    auto playerHandle = player(registry);
    auto &ball = ballHandle.get<Ball>();
    ball.sticky = true;
    if (auto *ptr = gamePlayRegistry.try_ctx<GameplayContext>(); ptr)
    {
      registry.emplace_or_replace<Graphic::RenderSprite>(playerHandle.entity(),
                                                         ptr->player_sprite,               // rotate
                                                         ptr->player_render_group,         // render groupe
                                                         glm::vec4(1.0f, 0.5f, 1.0f, 1.0f) // color
      );
    }
  }
  void desactive_sticky(entt::registry &registry)
  {
    auto ballHandle = ball(registry);
    auto playerHandle = player(registry);
    auto &ball = ballHandle.get<Ball>();
    ball.sticky = false;
    if (auto *ptr = gamePlayRegistry.try_ctx<GameplayContext>(); ptr)
    {
      registry.emplace_or_replace<Graphic::RenderSprite>(playerHandle.entity(),
                                                         ptr->player_sprite,               // rotate
                                                         ptr->player_render_group,         // render groupe
                                                         glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) // color
      );
    }
  }
  void active_passthrough(entt::registry &registry)
  {
    auto ballHandle = ball(registry);
    auto playerHandle = player(registry);
    auto &ball = ballHandle.get<Ball>();
    ball.passThrough = true;
    if (auto *ptr = gamePlayRegistry.try_ctx<GameplayContext>(); ptr)
    {
      registry.emplace_or_replace<Graphic::RenderSprite>(playerHandle.entity(),
                                                         ptr->player_sprite,               // rotate
                                                         ptr->player_render_group,         // render groupe
                                                         glm::vec4(1.0f, 0.5f, 0.5f, 1.0f) // color
      );
    }
  }
  void desactive_passthrough(entt::registry &registry)
  {
    auto ballHandle = ball(registry);
    auto playerHandle = player(registry);
    auto &ball = ballHandle.get<Ball>();
    ball.passThrough = false;
    if (auto *ptr = gamePlayRegistry.try_ctx<GameplayContext>(); ptr)
    {
      registry.emplace_or_replace<Graphic::RenderSprite>(playerHandle.entity(),
                                                         ptr->player_sprite,               // rotate
                                                         ptr->player_render_group,         // render groupe
                                                         glm::vec4(1.0f, 1.0f, 0.5f, 1.0f) // color
      );
    }
  }
  void active_padsize_increase(entt::registry &registry)
  {
    auto playerHandle = player(registry);
    auto &transform = playerHandle.get<Graphic::Transform>();
    transform.size.x = PLAYER_SIZE.x + 50;
  }
  void desactive_padsize_increase(entt::registry &registry)
  {
    auto playerHandle = player(registry);
    auto &transform = playerHandle.get<Graphic::Transform>();
    transform.size.x = PLAYER_SIZE.x;
  }
  void active_speed_power_up(entt::registry &registry)
  {
    auto playerHandle = player(registry);
    auto &movable = playerHandle.get<Gameplay::Movable>();
    movable.speed = PLAYER_SPEED * 1.2f;
  }
  void desactive_speed_power_up(entt::registry &registry)
  {
    auto playerHandle = player(registry);
    auto &movable = playerHandle.get<Gameplay::Movable>();
    movable.speed = PLAYER_SPEED;
  }
  void active_powerup(entt::registry &registry, PowerUp powerup)
  {
    std::cout << "Active power up " << debug_powerup(powerup) << std::endl;
    auto p = player(registry);
    auto soundToPlay = Audio::Play2D("powerup"_hs, false, 4500);
    if (registry.all_of<Audio::Play2D>(p) == false)
    {
      registry.emplace<Audio::Play2D>(p, soundToPlay);
    }
    else
    {
      registry.replace<Audio::Play2D>(p, soundToPlay);
    }
    switch (powerup.type)
    {
    case PowerUp::Type::Chaos:
      Graphic::PostProcessing::active_effect("chaos"_hs);
      break;
    case PowerUp::Type::Confuse:
      Graphic::PostProcessing::active_effect("invert"_hs);
      break;
    case PowerUp::Type::PadSizeIncrease:
      active_padsize_increase(registry);
      break;
    case PowerUp::Type::PassThrough:
      active_passthrough(registry);
      break;
    case PowerUp::Type::Speed:
      active_speed_power_up(registry);
      break;
    case PowerUp::Type::Sticky:
      active_sticky(registry);
      break;
    default:
      break;
    }
  }
  void desactive_powerup(entt::registry &registry, PowerUp powerup)
  {
    std::cout << "Desactive power up " << debug_powerup(powerup) << std::endl;
    switch (powerup.type)
    {
    case PowerUp::Type::Chaos:
      Graphic::PostProcessing::desactive_effect("chaos"_hs);
      break;
    case PowerUp::Type::Confuse:
      Graphic::PostProcessing::desactive_effect("invert"_hs);
      break;
    case PowerUp::Type::PadSizeIncrease:
      desactive_padsize_increase(registry);
      break;
    case PowerUp::Type::PassThrough:
      desactive_passthrough(registry);
      break;
    case PowerUp::Type::Speed:
      desactive_speed_power_up(registry);
      break;
    case PowerUp::Type::Sticky:
      desactive_sticky(registry);
      break;
    default:
      break;
    }
  }
  void on_powerup(entt::registry &registry, PowerUp powerup)
  {

    /*     std::cout << "On power up " << debug_powerup(powerup) << std::endl; */
  }
  void uodate_powerup(entt::registry &registry, float dt)
  {
    auto view = registry.view<PowerUp, Graphic::Target, Duration>();
    auto p = player(registry);
    for (auto entity : view)
    {
      auto [powerup, target, duration] = view.get(entity);
      if (target.target == p.entity())
      {

        if (duration.value > 0)
        {
          if (powerup.active == false)
          {
            powerup.active = true;
            active_powerup(registry, powerup);
          }
          else
          {
            on_powerup(registry, powerup);
          }
        }
        else if (powerup.active == true)
        {
          powerup.active = false;
          desactive_powerup(registry, powerup);
        }
      }
    }
  }
  void update(entt::registry &registry, float dt)
  {
    update_duration(registry, dt);
    update_shake(registry, dt);
    uodate_powerup(registry, dt);
    mouvement(
        registry, [&registry](entt::entity entity) {
          if (registry.all_of<Ball>(entity))
          {
            reset_player(registry);
            reset_level(registry);
          }
          else
          {
            registry.emplace<Graphic::Destroy>(entity);
          }
        });
    on_collision(registry);
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
      for (auto e : registry.view<Brick>())
      {
        registry.destroy(e);
      }
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
                position.value.x -= velocity.speed.x * dt;
              }
            });
      }
      if (action == GLFW_KEY_RIGHT)
      {
        registry.patch<Graphic::Position>(
            player, [dt, velocity, world, transform](Graphic::Position &position) {
              if (position.value.x <= world.viewport.x - transform.size.x)
                position.value.x += velocity.speed.x * dt;
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