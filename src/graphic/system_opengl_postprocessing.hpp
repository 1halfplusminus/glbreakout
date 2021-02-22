
#include "system_postprocessing.cpp"
#include "opengl.hpp"
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
        }
        void init(entt::registry &registry, unsigned int w, unsigned int h, const std::string vertexShader, std::string fragmentShader)
        {

            auto framebuffer_vs_source = Graphic::load_shader_source(
                "framebuffer_vs"_hs, vertexShader, Graphic::VertexShader);
            auto framebuffer_fs_source = Graphic::load_shader_source(
                "framebuffer_fs"_hs, fragmentShader, Graphic::FragmentShader);

            auto framebuffer_vs = Graphic::load_shader("framebuffer_vs"_hs, framebuffer_vs_source);
            auto framebuffer_fs = Graphic::load_shader("framebuffer_fs"_hs, framebuffer_fs_source);

            shader = Graphic::load_shader_program(
                         "framebuffer"_hs, std::vector<Graphic::Shader>{framebuffer_vs, framebuffer_fs})
                         .get();
            post_processing = std::unique_ptr<OpenGL::PostProcessing>(new OpenGL::PostProcessing(w, h));
            initalized = true;
        }
        void update(entt::registry &registry, float dt)
        {
            assert(initalized == true);
            post_processing->Capture();
        }
        void render(entt::registry &registry)
        {
            assert(initalized == true);
            post_processing->Render(shader);
        }
    }
} // namespace Graphic
