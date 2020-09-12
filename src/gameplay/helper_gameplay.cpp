#include "gameplay/helper_gameplay.hpp"
#include "system/system_graphic_pipeline.hpp"
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
namespace Gameplay
{
    namespace
    {

        entt::registry gamePlayRegistry;
        void handle_error_context()
        {
            std::cout << "gameplay context not initialized !";
            exit;
        }
        void render_player(unsigned int w, unsigned int h)
        {
            if (auto *ptr = gamePlayRegistry.try_ctx<GameplayContext>(); ptr)
            {
                // init player;
                const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
                glm::vec3 playerPos = glm::vec3(
                    w / 2.0f - PLAYER_SIZE.x / 2.0f,
                    h - PLAYER_SIZE.y - 10, 1.0f);
                Graphic::create_sprite(ptr->player_render_group, ptr->player_sprite, playerPos, PLAYER_SIZE, 0.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                return;
            }
            handle_error_context();
        }
        void render_backgroud(unsigned int w, unsigned int h)
        {
            if (auto *ptr = gamePlayRegistry.try_ctx<GameplayContext>(); ptr)
            {
                Graphic::create_sprite(ptr->backgroud_render_group, ptr->backgroud_sprite, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), glm::vec3(w, h, 0.0f), 0.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                return;
            }
            handle_error_context();
        }
    } // namespace
    std::shared_ptr<Level>
    level_loader::load(const std::string &filePath, unsigned int lvlWidth, unsigned int lvlHeight) const
    {
        std::shared_ptr<Level> level = std::shared_ptr<Level>(new Level());
        std::ifstream file(filePath);
        std::string line;
        std::vector<BrickType> types = {
            BrickType::BREAKABLE,
            BrickType::BREAKABLE,
            BrickType::SOLID,
            BrickType::SOLID,
            BrickType::SOLID,
            BrickType::SOLID};
        std::vector<glm::vec4> colors = {
            glm::vec4(0.0f, 0.0f, 0.0f, 0.0f),
            glm::vec4(0.8f, 0.8f, 0.7f, 1.0f),
            glm::vec4(0.2f, 0.6f, 1.0f, 1.0f),
            glm::vec4(0.0f, 0.7f, 0.0f, 1.0f),
            glm::vec4(0.8f, 0.8f, 0.4f, 1.0f),
            glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)};
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

        auto atlas_image = Graphic::load_image("atlas"_hs, "./texture/atlas.png", false);
        auto backgroud_image = Graphic::load_image("backgroud"_hs, "./texture/background.jpg", false);

        auto &atlas = Graphic::load_texture("atlas"_hs, atlas_image);
        auto &background_texture = Graphic::load_texture("background"_hs, backgroud_image);

        context.backgroud_render_group = Graphic::create_render_group("main"_hs, "background"_hs);

        context.player_render_group = Graphic::create_render_group("main"_hs, "atlas"_hs);

        context.brick_render_group = Graphic::create_render_group("main"_hs, "atlas"_hs);

        context.brick_sprite_solid = Graphic::Sprite({atlas,
                                                      glm::vec4(0, 512, 128, 128)});
        context.brick_sprite_breakable = Graphic::Sprite({atlas,
                                                          glm::vec4(128, 512, 128, 128)});

        context.player_sprite = Graphic::Sprite({atlas,
                                                 glm::vec4(0, 640, 512, 128)});

        context.backgroud_sprite = Graphic::Sprite({background_texture,
                                                    glm::vec4(0, 0, background_texture.get().width, background_texture.get().height)});

        Gameplay::load_level("1"_hs, "./assets/one.txt", w, h / 2);
        Gameplay::load_level("2"_hs, "./assets/two.txt", w, h / 2);
        Gameplay::load_level("3"_hs, "./assets/three.txt", w, h / 2);
        Gameplay::load_level("4"_hs, "./assets/four.txt", w, h / 2);

        Gameplay::render_backgroud(w, h);
        Gameplay::render_player(w, h);
        Gameplay::render_level("4"_hs);
    }
    entt::resource_handle<Level> load_level(entt::hashed_string &levelId, const std::string &path, unsigned int lvlWidth, unsigned int lvlHeight)
    {
        if (auto *ptr = gamePlayRegistry.try_ctx<GameplayContext>(); ptr)
        {
            auto level = ptr->level_cache.load<level_loader>(levelId, path, lvlWidth, lvlHeight);
            return level;
        }
        handle_error_context();
    }
    void render_level(entt::hashed_string &levelId)
    {
        if (auto *ptr = gamePlayRegistry.try_ctx<GameplayContext>(); ptr)
        {
            Graphic::clear_render_group(ptr->brick_render_group);
            auto &level = ptr->level_cache.handle(levelId).get();
            for (int i = 0; i < level.bricks.size(); i++)
            {
                auto sprite = (level.bricks[i] == Gameplay::BrickType::BREAKABLE) ? ptr->brick_sprite_breakable : ptr->brick_sprite_solid;
                Graphic::create_sprite(ptr->brick_render_group, sprite, glm::vec3(level.positions[i], 0.0f), glm::vec2(level.unit_width, level.unit_height), 0.0f, level.colors[i]);
            }
            return;
        }
        handle_error_context();
    }
} // namespace Gameplay