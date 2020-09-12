

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
#include "gameplay/component_gameplay.hpp"
#include "gameplay/helper_gameplay.hpp"

void Game::processInput(GLFWwindow *window, Registry &registry, float dt) {}
void Game::render(Registry &registry) {}
void Game::update(Registry &registry, float dt)
{
    Graphic::update(registry, dt);
}
void Game::init(Registry &registry, float w, float h)
{

    Graphic::init(registry);
    Gameplay::init(registry, w, h);
    auto main_vs_source = Graphic::load_shader_source("main_vs"_hs, "./shader/main.vect", Graphic::VertexShader);
    auto main_fs_source = Graphic::load_shader_source("main_fs"_hs, "./shader/main.frag", Graphic::FragmentShader);

    auto main_vs = Graphic::load_shader("main_vs"_hs, main_vs_source);
    auto main_fs = Graphic::load_shader("main_fs"_hs, main_fs_source);

    auto main_shader_r = Graphic::load_shader_program("main"_hs,
                                                      std::vector<Graphic::Shader>{main_vs,
                                                                                   main_fs});
    Graphic::add_projection_matrix(glm::ortho(0.0f, static_cast<float>(w), static_cast<float>(h),
                                              0.0f, -1.0f, 1.0f));
}