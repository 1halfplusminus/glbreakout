#include "gameplay/helper_gameplay.hpp"
#include "game.h"
#include "gameplay/component_gameplay.hpp"
#include "graphic/system_graphic_pipeline.hpp"
#include "physic/component_physic.hpp"
#include <GLFW/glfw3.h>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <sstream>
#include <vector>

namespace Gameplay {

namespace {
constexpr glm::vec3 VELOCITY_INITIAL(100.0f, -350.0f, 0.0f);
constexpr glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
entt::registry gamePlayRegistry;
Direction vector_direction(glm::vec2 target) {
  glm::vec2 compass[] = {
      glm::vec2(0.0f, 1.0f),  // up
      glm::vec2(1.0f, 0.0f),  // right
      glm::vec2(0.0f, -1.0f), // down
      glm::vec2(-1.0f, 0.0f)  // left
  };
  float max = 0.0f;
  unsigned int best_match = -1;
  for (unsigned int i = 0; i < 4; i++) {
    float dot_product = glm::dot(glm::normalize(target), compass[i]);
    if (dot_product > max) {
      max = dot_product;
      best_match = i;
    }
  }
  return (Direction)best_match;
}

void handle_error_context() {
  std::cout << "gameplay context not initialized !";
  exit(1);
}
entt::basic_handle<entt::entity> game_state(entt::registry &registry) {
  auto games = registry.view<BreakoutGame>();
  if (games.size() > 0) {
    return entt::basic_handle(registry, games[0]);
  }
  std::cout << "No player initialized \n";
  return entt::basic_handle(registry, static_cast<entt::entity>(entt::null));
}
entt::basic_handle<entt::entity> player(entt::registry &registry) {
  auto players = registry.view<entt::tag<Gameplay::player_tag>>();
  if (players.size() > 0) {
    return entt::basic_handle(registry, players[0]);
  }
  std::cout << "No player initialized \n";
  return entt::basic_handle(registry, static_cast<entt::entity>(entt::null));
}
entt::basic_handle<entt::entity> ball(entt::registry &registry) {
  auto balls = registry.view<Ball>();
  if (balls.size() > 0) {
    return entt::basic_handle(registry, balls[0]);
  }
  std::cout << "No ball initialized \n";
  return entt::basic_handle(registry, static_cast<entt::entity>(entt::null));
}
void create_ground(entt::registry &registry,glm::vec3 pos, unsigned int w, unsigned int h) {
  
  if (auto *ptr = gamePlayRegistry.try_ctx<GameplayContext>(); ptr) {
    auto ground = registry.create();
    const glm::vec2 size(w, h);
    registry.emplace<Graphic::Position>(ground, pos);
    registry.emplace<Physic::RigidBody>(ground, 1 << 1, 1 << 0);
    registry.emplace<Physic::AABB>(ground, pos + glm::vec3(static_cast<float>(w), static_cast<float>(h),0.0f), pos);
    auto sprite =  ptr->brick_sprite_breakable;
    registry.emplace<Graphic::RenderSprite>(
          ground, sprite, ptr->player_render_group, glm::vec4(1.0f,0.0f,0.0f,1.0f));
    registry.emplace<Graphic::Transform>(ground, 0.0f,size);
  }
  return;
}

void render_ground(entt::registry &registry, unsigned int w, unsigned int h) {
  // bottom ground
  create_ground(registry,glm::vec3(0.0f,h-10.0f,0.0f),w,10);
  // top
  create_ground(registry,glm::vec3(0.0f,0.0f,0.0f),w,10);
  // left
  create_ground(registry,glm::vec3(0,0.0f,0.0f),10,h);
   // right
  create_ground(registry,glm::vec3(w-10,0.0f,0.0f),10,h);
}
void render_ball(entt::registry &registry, unsigned int w, unsigned int h) {
  if (auto *ptr = gamePlayRegistry.try_ctx<GameplayContext>(); ptr) {
    const float ballRadius = 12.5f;
    auto [playerPosition, transform, movable] =
        player(registry).get<Graphic::Position, Graphic::Transform, Movable>();
    const glm::vec2 size(ballRadius * 2, ballRadius * 2);

    glm::vec3 pos =
        playerPosition.value + glm::vec3(transform.size.x / 2.0f - ballRadius,
                                         -ballRadius * 2.0f, 0.0f);

    auto ball = registry.create();
    registry.emplace<Ball>(ball);
    registry.emplace<Graphic::Position>(ball, pos);
    registry.emplace<Velocity>(ball, glm::vec3(0.0f));
    registry.emplace<Movable>(ball, movable.speed);
    registry.emplace<Graphic::Transform>(ball, 0.0f, size);
    registry.emplace<Graphic::RenderSprite>(ball, ptr->ball_sprite,
                                            ptr->player_render_group,
                                            glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    registry.emplace<Physic::RigidBody>(ball, 1 << 0, 1 << 1);
    registry.emplace<Physic::SphereCollider>(ball, ballRadius);
    return;
  }
  handle_error_context();
}
void render_player(entt::registry &registry, unsigned int w, unsigned int h) {
  if (auto *ptr = gamePlayRegistry.try_ctx<GameplayContext>(); ptr) {
    // init player;

    glm::vec3 playerPos = glm::vec3(w / 2.0f - PLAYER_SIZE.x / 2.0f,
                                    h - PLAYER_SIZE.y - 10.0f, 1.0f);

    auto player = registry.create();
    registry.emplace<entt::tag<Gameplay::player_tag>>(player);
    registry.emplace<Graphic::Position>(player, playerPos);
    registry.emplace<Graphic::Transform>(player, 0.0f, PLAYER_SIZE);
    registry.emplace<Graphic::RenderSprite>(player, ptr->player_sprite,
                                            ptr->player_render_group,
                                            glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    registry.emplace<Movable>(player, glm::vec3(500.0f, 0.0f, 0.0f));
    registry.emplace<Physic::RigidBody>(player, 1 << 2, 1 << 0);
    registry.emplace<Physic::AABB>(
        player, playerPos + glm::vec3(PLAYER_SIZE, 0.0f), playerPos);
    return;
  }
  handle_error_context();
}
void render_backgroud(entt::registry &registry, unsigned int w,
                      unsigned int h) {
  if (auto *ptr = gamePlayRegistry.try_ctx<GameplayContext>(); ptr) {
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
void on_ball_player_collision(entt::registry &registry, entt::entity a,
                              entt::entity b) {
  if (registry.has<Ball>(a) && (registry.has<entt::tag<player_tag>>(b))) {
    auto aOtherPosition = registry.get<Graphic::Position>(b);
    auto ballPosition = registry.get<Graphic::Position>(a);
    auto balLTransform = registry.get<Graphic::Transform>(a);
    auto otherTransform = registry.get<Graphic::Transform>(b);
    auto velocity = registry.get<Velocity>(a);
    auto sphere = registry.get<Physic::SphereCollider>(a);

    auto ballCenter = ballPosition.value +
                      (glm::vec3(balLTransform.size, 1.0f) / glm::vec3(2));
    auto aOtherCenter = aOtherPosition.value +
                        (glm::vec3(otherTransform.size, 1.0f) / glm::vec3(2));
    float distance = (ballPosition.value.x + sphere.radius) - aOtherCenter.x;
    float percentage = distance / (otherTransform.size.x / 2.0f);

    float strength = 2.0f;
    glm::vec2 oldVelocity = velocity.value;

    velocity.value.x = VELOCITY_INITIAL.x * percentage * strength;
    velocity.value.y = -velocity.value.y;
    velocity.value = glm::normalize(velocity.value) * glm::length(oldVelocity);
    registry.patch<Velocity>(
        a, [velocity](Velocity &v) { v.value = velocity.value; });
  }
}
void on_ball_brick_collision(entt::registry &registry, entt::entity a,
                             entt::entity b) {
  if (registry.has<Ball>(a) && registry.has<Brick>(b)) {
    auto brick = registry.get<Brick>(b);
    if (brick.type != BrickType::SOLID) {
      registry.patch<Brick>(b, [](Brick &brick) { brick.destroyed = true; });
      registry.emplace<Graphic::Destroy>(b);
      registry.remove_if_exists<Physic::RigidBody>(b);
    }
  }
  if (registry.has<Ball>(a) && (registry.has<Brick>(b))) {
    auto aOtherPosition = registry.get<Graphic::Position>(b);
    auto ballPosition = registry.get<Graphic::Position>(a);
    auto balLTransform = registry.get<Graphic::Transform>(a);
    auto otherTransform = registry.get<Graphic::Transform>(b);
    auto velocity = registry.get<Velocity>(a);
    auto sphere = registry.get<Physic::SphereCollider>(a);
    auto ballCenter = ballPosition.value +
                      (glm::vec3(balLTransform.size, 1.0f) / glm::vec3(2));
    auto aOtherCenter = aOtherPosition.value +
                        (glm::vec3(otherTransform.size, 1.0f) / glm::vec3(2));
    glm::vec3 centers_distance = glm::normalize(aOtherCenter - ballCenter);
    Direction dir = vector_direction(glm::vec2(centers_distance));
    if (dir == LEFT || dir == RIGHT) {
      registry.patch<Velocity>(
          a, [velocity](Velocity &v) { v.value.x = -v.value.x; });
      float penetrationValue = sphere.radius - std::abs(centers_distance.x);
      if (dir == LEFT) {
        registry.patch<Graphic::Position>(
            a, [penetrationValue](Graphic::Position &p) {
              p.value.x += penetrationValue;
            });
      } else {
        registry.patch<Graphic::Position>(
            a, [penetrationValue](Graphic::Position &p) {
              p.value.x -= penetrationValue;
            });
      }
    } else {
      registry.patch<Velocity>(
          a, [velocity](Velocity &v) { v.value.y = -v.value.y; });
      float penetrationValue = sphere.radius - std::abs(centers_distance.y);
      if (dir == UP) {
        registry.patch<Graphic::Position>(
            a, [penetrationValue](Graphic::Position &p) {
              p.value.y -= penetrationValue;
            });
      } else {
        registry.patch<Graphic::Position>(
            a, [penetrationValue](Graphic::Position &p) {
              p.value.y += penetrationValue;
            });
      }
    }
  }
}
void on_collision(entt::registry &registry) {
  auto view = registry.view<Physic::Collision>();
  if (view.size() > 0) {
    for (auto entity : view) {
      auto collision = view.get<Physic::Collision>(entity);
      on_ball_brick_collision(registry, collision.a, collision.b);
      on_ball_brick_collision(registry, collision.b, collision.a);
      on_ball_player_collision(registry, collision.a, collision.b);
      on_ball_player_collision(registry, collision.b, collision.a);
    }
  }
}
void reset_level(entt::registry &registry) {
  auto gameHandle = game_state(registry);
  render_level(registry, gameHandle.get<BreakoutGame>().currentLevel);
}
void reset_player(entt::registry &registry) {
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
  glm::vec3 ballPosition = playerPosition.value +
                           glm::vec3(transform.size.x / 2.0f - collision.radius,
                                     -collision.radius * 2.0f, 0.0f);
  ballHandle.replace<Graphic::Position>(ballPosition);
  ballHandle.emplace<Velocity>(glm::vec3(0.0f));
  ballHandle.emplace<Movable>(movable.speed);
}
void ball_mouvement(entt::registry &registry) {
  auto world = Game::world(registry).get<Game::World>();
  auto balls =
      registry.view<Ball, Graphic::Position, Graphic::Transform>(
          entt::exclude<Movable>);
  for (auto entity : balls) {
    auto transform = registry.get<Graphic::Transform>(entity);
    auto position = registry.get<Graphic::Position>(entity);
    if (position.value.y + transform.size.y >=
                world.viewport.y - 10 ) {
      reset_player(registry);
      reset_level(registry);
    }
  }
}

} // namespace
std::shared_ptr<Level> level_loader::load(const std::string &filePath,
                                          unsigned int lvlWidth,
                                          unsigned int lvlHeight) const {
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
  while (std::getline(file, line)) {
    std::stringstream lineStream(line);
    int value;
    level->rows = 0;
    while (lineStream >> value) {
      BrickType brick;
      glm::vec4 color;
      if (value < types.size()) {
        brick = types[value];
        color = colors[value];
      }
      if (value != 0) {
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
  for (auto &position : level->positions) {
    position.x *= level->unit_width;
    position.y *= level->unit_height;
  }
  return level;
}
void init(entt::registry &registry, unsigned int w, unsigned int h) {
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

  // create game state
  auto gameEntity = registry.create();
  auto gameState = registry.emplace<BreakoutGame>(gameEntity, "1"_hs);
  Gameplay::load_level("1"_hs, "./assets/one.txt", w, h / 2);
  Gameplay::load_level("2"_hs, "./assets/two.txt", w, h / 2);
  Gameplay::load_level("3"_hs, "./assets/three.txt", w, h / 2);
  Gameplay::load_level("4"_hs, "./assets/four.txt", w, h / 2);

  Gameplay::render_backgroud(registry, w, h);
  Gameplay::render_level(registry, gameState.currentLevel);
  Gameplay::render_player(registry, w, h);
  Gameplay::render_ball(registry, w, h);
  Gameplay::render_ground(registry, w, h);
}
void update(entt::registry &registry) {
  on_collision(registry);
  ball_mouvement(registry);
}
entt::resource_handle<Level> load_level(entt::hashed_string &levelId,
                                        const std::string &path,
                                        unsigned int lvlWidth,
                                        unsigned int lvlHeight) {
  if (auto *ptr = gamePlayRegistry.try_ctx<GameplayContext>(); ptr) {
    auto level =
        ptr->level_cache.load<level_loader>(levelId, path, lvlWidth, lvlHeight);
    return level;
  }
  handle_error_context();
}

void render_level(entt::registry &registry, entt::hashed_string &levelId) {
  if (auto *ptr = gamePlayRegistry.try_ctx<GameplayContext>(); ptr) {
    Graphic::clear_render_group(ptr->brick_render_group);
    for (auto e : registry.view<Brick>()) {
      registry.destroy(e);
    }
    auto &level = ptr->level_cache.handle(levelId).get();
    for (int i = 0; i < level.bricks.size(); i++) {
      auto brick = registry.create();
      auto pos = glm::vec3(level.positions[i], 0.0f);
      auto size = glm::vec2(level.unit_width, level.unit_height);
      registry.emplace<Graphic::Position>(brick, pos);
      registry.emplace<Graphic::Transform>(brick, 0.0f, size);
      auto sprite = (level.bricks[i] == Gameplay::BrickType::BREAKABLE)
                        ? ptr->brick_sprite_breakable
                        : ptr->brick_sprite_solid;
      registry.emplace<Graphic::RenderSprite>(
          brick, sprite, ptr->brick_render_group, level.colors[i]);

      registry.emplace<Physic::RigidBody>(brick, 1 << 1, 1 << 0);
      registry.emplace<Physic::AABB>(brick, pos + glm::vec3(size, 0), pos);
      registry.emplace<Brick>(brick, level.bricks[i], false);

      auto gameHandle = game_state(registry);
      gameHandle.replace<BreakoutGame>(levelId);
    }
    return;
  }
  handle_error_context();
}
void processInput(Registry &registry, float dt, int action) {
  auto players =
      registry.view<Movable, Graphic::Position, Graphic::Transform>();
  auto world = Game::world(registry).get<Game::World>();
  for (auto player : players) {
    auto velocity = players.get<Movable>(player);
    auto transform = players.get<Graphic::Transform>(player);
    if (action == GLFW_KEY_A) {
      registry.patch<Graphic::Position>(
          player, [dt, velocity, world](Graphic::Position &position) {
            if (position.value.x >= 0.0f) {
              position.value.x -= velocity.speed.x * world.deltaTime;
            }
          });
    }
    if (action == GLFW_KEY_D) {
      registry.patch<Graphic::Position>(
          player,
          [dt, velocity, world, transform](Graphic::Position &position) {
            if (position.value.x <= world.viewport.x - transform.size.x)
              position.value.x += velocity.speed.x * world.deltaTime;
          });
    }
  }
  if (action == GLFW_KEY_SPACE) {
    auto ballHandle = ball(registry);
    if(ballHandle.has<Movable>()) {
      ballHandle.remove<Movable>();
      ballHandle.emplace_or_replace<Velocity>(VELOCITY_INITIAL);
    }
  }
}
} // namespace Gameplay