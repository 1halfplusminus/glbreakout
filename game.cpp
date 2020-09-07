

#include "game.h"
#include "component/component_graphic_pipeline.hpp"
#include "graphic/graphic_image.hpp"
#include "graphic/graphic_texture.hpp"
#include "helper/helper_graphic_pipeline.hpp"
#include "system/system_graphic_pipeline.hpp"
#include <entt/entt.hpp>

namespace Game {
void init(Registry &registry) {

  entt::resource_cache<Graphic::ShaderSource> shader_source_cache;
  shader_source_cache.load<Graphic::shader_source_loader>(
      "main_vs"_hs, "./shader/main.vect", Graphic::VertexShader);
  shader_source_cache.load<Graphic::shader_source_loader>(
      "main_fs"_hs, "./shader/main.frag", Graphic::FragmentShader);

  entt::resource_cache<Graphic::Shader> shader_cache;
  shader_cache.load<Graphic::shader_loader>(
      "main_vs"_hs, shader_source_cache.handle("main_vs"_hs));
  shader_cache.load<Graphic::shader_loader>(
      "main_fs"_hs, shader_source_cache.handle("main_fs"_hs));

  entt::resource_cache<Graphic::ShaderProgam> program_cache;
  program_cache.load<Graphic::shader_program_loader>(
      "main"_hs,
      std::vector<Graphic::Shader>{shader_cache.handle("main_vs"_hs),
                                   shader_cache.handle("main_fs"_hs)});

  entt::resource_cache<Graphic::Image> image_cache;
  image_cache.load<Graphic::image_loader>("atlas"_hs, "./texture/atlas.png");

  entt::resource_cache<Graphic::Texture> texture_cache;
  texture_cache.load<Graphic::texture_image_loader>(
      "atlas"_hs, image_cache.handle("atlas"_hs));

  Graphic::init(registry);
}
} // namespace Game