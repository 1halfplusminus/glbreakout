

// clang-format off
#include <glad/glad.h>
// clang-format on
#include <GLFW/glfw3.h>
#include <iostream>
#include <entt/entt.hpp>
#include "constants.h"
#include "game.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods);

int main() {
  // Init GLFW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Breakout", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  /*   glfwSwapInterval(0); */
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  // INIT GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  // Create our registry
  auto registry = std::make_unique<entt::registry>();
  // initialize the game
  Game::init(*registry, SCR_WIDTH, SCR_HEIGHT);

  auto lastFrame = glfwGetTime();

  glfwSetWindowUserPointer(window, registry.get());

  glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode,
                                int action, int mods) {
    auto registry = static_cast<Registry *>(glfwGetWindowUserPointer(window));
    Game::processInput(*registry, key);
  });
  while (!glfwWindowShouldClose(window)) {
    auto currentFrame = glfwGetTime();
    auto deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    Game::update(*registry, static_cast<float>(deltaTime));
    Game::render(*registry);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}
