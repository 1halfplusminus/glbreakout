

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
void Game::update(Registry &registry, float dt)
{
    Graphic::update(registry, dt);
}
void Game::init(Registry &registry, float w, float h)
{

    Graphic::init(registry);
    auto main_vs_source = Graphic::load_shader_source("main_vs"_hs, "./shader/main.vect", Graphic::VertexShader);
    auto main_fs_source = Graphic::load_shader_source("main_fs"_hs, "./shader/main.frag", Graphic::FragmentShader);

    auto main_vs = Graphic::load_shader("main_vs"_hs, main_vs_source);
    auto main_fs = Graphic::load_shader("main_fs"_hs, main_fs_source);

    auto main_shader_r = Graphic::load_shader_program("main"_hs,
                                                      std::vector<Graphic::Shader>{main_vs,
                                                                                   main_fs});

    auto atlas_image = Graphic::load_image("atlas"_hs, "./texture/atlas.png", false);

    auto &atlas = Graphic::load_texture("atlas"_hs, atlas_image);

    // register shaderprogram

    entt::basic_handle<entt::entity> handle{registry};
    std::vector<Graphic::VertexData> data;
    data.push_back(Graphic::VertexData{glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)});
    data.push_back(Graphic::VertexData{glm::vec4(1.0f, 0.0f, 1.0f, 0.0f)});
    data.push_back(Graphic::VertexData{glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)});
    data.push_back(Graphic::VertexData{glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)});
    data.push_back(Graphic::VertexData{glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)});
    data.push_back(Graphic::VertexData{glm::vec4(1.0f, 0.0f, 1.0f, 0.0f)});
    int verticesIndex = 0;
    Graphic::Sprite renderable = Graphic::Sprite({atlas,
                                                  glm::vec4(0, 640, 512, 128)});
    // create player
    for (int i = 0; i < 2000; i++)
    {
        auto player = registry.create();
        registry.emplace<Graphic::Sprite>(player, renderable);
        auto position = registry.emplace<Graphic::Position>(player, glm::vec3(200.0f, 200.0f, 0.0f));
        auto transform = registry.emplace<Graphic::Transform>(player, 45.0f, glm::vec2(512.0f, 128.0f));
        registry.emplace<Graphic::Material>(
            player, main_shader_r.get().id);

        for (int y = 0; y < data.size(); y++)
        {
            auto vertex = registry.create();
            float offsetX =
                ((2 * (renderable.rect.x)) + 1) / (2 * renderable.texture.width);
            float offsetY =
                ((2 * (renderable.rect.y)) + 1) / (2 * renderable.texture.height);
            float ux = ((2 * (renderable.rect.z)) + 1) / (2 * renderable.texture.width);
            float uy =
                ((2 * (renderable.rect.w)) + 1) / (2 * renderable.texture.height);
            glm::vec2 uv;
            uv.x = (data[y].vertice.x * ux) + offsetX;
            uv.y = (data[y].vertice.y * uy) + offsetY;
            glm::vec4 color;
            color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
            // model
            glm::mat4 model = glm::mat4(1.0f);
            auto rotate = transform.rotate;
            auto size = transform.size;
            model = glm::translate(model,
                                   glm::vec3(position.value.x, position.value.y, 0.0f));
            model =
                glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
            model =
                glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
            model =
                glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

            model = glm::scale(model, glm::vec3(size, 1.0f));

            registry.emplace<Graphic::VertexData>(vertex, data[y].vertice, uv, model, color);
        }
    }
    // create camera
    auto camera = registry.create();
    registry.emplace<Graphic::ProjectionMatrix>(
        camera, glm::ortho(0.0f, static_cast<float>(w), static_cast<float>(h),
                           0.0f, -1.0f, 1.0f));
}