
#include "system_postprocessing.hpp"
#include "opengl.cpp"
#include <assert.h>
#include "graphic/helper_graphic_pipeline.hpp"
#include "graphic/system_graphic_pipeline.hpp"

namespace Graphic
{
    namespace PostProcessing
    {
        namespace
        {
            bool initalized;
            ShaderProgam shader;
            std::unique_ptr<OpenGL::PostProcessing> post_processing;
            static const entt::hashed_string default_shader_key = "postprocessing"_hs;
            float time;
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
            shader = Graphic::get_shader_program(key).get();
        }
        void desactive_effect()
        {
            active_effect(default_shader_key);
        }
        void init(entt::registry &registry, unsigned int w, unsigned int h, const std::string vertexShader, const std::string fragmentShader)
        {

            load_effect(default_shader_key, vertexShader, fragmentShader);
            active_effect(default_shader_key);

            post_processing = std::unique_ptr<OpenGL::PostProcessing>(new OpenGL::PostProcessing(w, h));
            initalized = true;
        }
        void update(entt::registry &registry, float dt)
        {
            assert(initalized == true);
            time = dt;
            post_processing->Capture();
        }
        void render(entt::registry &registry)
        {
            assert(initalized == true);
            glUseProgram(shader);
            glUniform1f(glGetUniformLocation(shader, "time"), time);
            post_processing->Render(shader);
        }
    }
} // namespace Graphic
