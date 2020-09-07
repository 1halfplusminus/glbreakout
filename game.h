#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

// clang-format off
#include <glad/glad.h>
// clang-format on
#include "ecs.h"
#include <GLFW/glfw3.h>

namespace Game {
enum GameState { GAME_ACTIVE, GAME_MENU, GAME_WIN };
void init(Registry &registry, float w, float h);
void render(Registry &registry);
// game loop
void processInput(GLFWwindow *window, Registry &registry, float dt);
void updateSim(Registry &registry, float dt);
void update(Registry &registry, float dt);
}; // namespace Game

#endif