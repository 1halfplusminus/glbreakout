

// clang-format off
#include <glad/glad.h>
// clang-format on
#include <GLFW/glfw3.h>
#include <iostream>
#include <entt/entt.hpp>
#include "constants.h"
#include "game.h"

void GLAPIENTRY MessageCallback(GLenum source,
                                GLenum type,
                                GLuint id,
                                GLenum severity,
                                GLsizei length,
                                const GLchar *message,
                                const void *userParam)
{
  if (type == GL_DEBUG_TYPE_ERROR)
  {
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
  }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods);

int main()
{

  // Init GLFW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Get Started", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  /*   glfwSwapInterval(0); */
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  // INIT GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);
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
  while (!glfwWindowShouldClose(window))
  {
    auto currentFrame = glfwGetTime();
    auto deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    Game::update(*registry, static_cast<float>(deltaTime), static_cast<float>(currentFrame));
    Game::render(*registry);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}
