
#pragma once
#include "system_postprocessing.hpp"
#include "my_opengl.hpp"
#include <assert.h>
#include "graphic/helper_graphic_pipeline.hpp"
#include "graphic/system_graphic_pipeline.hpp"
#include <unordered_map>
namespace Graphic
{
    namespace PostProcessing
    {
        namespace
        {
            bool initalized;
            ShaderProgam shader;
            static const entt::hashed_string default_shader_key = "postprocessing"_hs;
            float time;
            float _h;
            float _w;
            std::unordered_map<int, std::unique_ptr<OpenGL::PostProcessing>> postProcessings;
            std::map<int, ShaderProgam> effects;
        }
        void load_effect(entt::hashed_string key, const std::string &vertexShaderPath, const std::string &fragShaderPath)
        {
            auto postprocessing_vs_source_key = entt::hashed_string((std::string(key.data()) + "_vs").c_str());
            auto postprocessing_vs_source = Graphic::load_shader_source(
                postprocessing_vs_source_key, vertexShaderPath, Graphic::VertexShader);
            auto postprocessing_fs_source_key = entt::hashed_string((std::string(key.data()) + "fs").c_str());
            auto postprocessing_fs_source = Graphic::load_shader_source(
                postprocessing_fs_source_key, fragShaderPath, Graphic::FragmentShader);

            auto postprocessing_vs = Graphic::load_shader(postprocessing_vs_source_key, postprocessing_vs_source);
            auto postprocessing_fs = Graphic::load_shader(postprocessing_fs_source_key, postprocessing_fs_source);

            auto postprocessing_shader_r = Graphic::load_shader_program(
                key, std::vector<Graphic::Shader>{postprocessing_vs, postprocessing_fs});
        }
        void active_effect(entt::hashed_string key)
        {
            if (!initalized)
                return;
            assert(initalized == true);
            if (effects.find(key) == effects.end())
            {
                effects[key] = Graphic::get_shader_program(key).get();
            }
            if (postProcessings.find(key) == postProcessings.end())
            {
                postProcessings[key] = std::unique_ptr<OpenGL::PostProcessing>(new OpenGL::PostProcessing(_w, _h));
            }
        }
        void desactive_effect()
        {
            if (!initalized)
                return;
            assert(initalized == true);
            effects.clear();
        }
        void desactive_effect(entt::hashed_string key)
        {
            if (!initalized)
                return;
            assert(initalized == true);
            if (effects.find(key) != effects.end())
            {
                effects.erase(key);
            }
        }
        void init(entt::registry &registry, unsigned int w, unsigned int h, const std::string vertexShader, const std::string fragmentShader)
        {
            initalized = true;
            _h = h;
            _w = w;
            load_effect(default_shader_key, vertexShader, fragmentShader);
            active_effect(default_shader_key);
            shader = Graphic::get_shader_program(default_shader_key).get();
        }
        void update(entt::registry &registry, float dt)
        {
            assert(initalized == true);
            time = dt;
            postProcessings[default_shader_key]->Capture();
        }
        void render(entt::registry &registry)
        {
            assert(initalized == true);
            ShaderProgam shaderP;
            auto i = 0;
            OpenGL::PostProcessing *previous = nullptr;
            glUseProgram(shader);
            glUniform1f(glGetUniformLocation(shader, "time"), time);
            postProcessings[default_shader_key]->Render(shader, true);
            for (auto iter = effects.begin()++; iter != effects.end(); iter++)
            {
                shaderP = iter->second;
                auto effect = postProcessings[iter->first].get();
                effect->Bind(OpenGL::Framebuffer::Target::DRAW);
                glUseProgram(shaderP);
                glUniform1f(glGetUniformLocation(shaderP, "time"), time);
                effect->Render(shaderP, postProcessings[default_shader_key]->GetTexture());
                effect->Bind(OpenGL::Framebuffer::Target::READ);
                postProcessings[default_shader_key]->Bind(OpenGL::Framebuffer::Target::DRAW);
                glUseProgram(shader);
                postProcessings[default_shader_key]->Render(shader, effect->GetTexture());
            }
            postProcessings[default_shader_key]->Render(shader, true);
        }
    }
} // namespace Graphic
