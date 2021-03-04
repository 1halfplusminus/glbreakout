#pragma once
#ifndef GRAPHIC_TEXTURE_HPP
#define GRAPHIC_TEXTURE_HPP

#include "ecs.h"
#include "graphic_image.hpp"
#include <glm/glm.hpp>

namespace Graphic
{
  struct Texture
  {
    unsigned int id;
    unsigned int height;
    unsigned int width;
  };
  struct Material
  {
    unsigned int shader;
  };
  struct texture_image_loader final
      : entt::resource_loader<texture_image_loader, Graphic::Texture>
  {
    std::shared_ptr<Graphic::Texture> load(Image &image) const;
  };
} // namespace Graphic

#endif