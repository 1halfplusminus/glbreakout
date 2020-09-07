#ifndef GRAPHIC_IMAGE_HPP
#define GRAPHIC_IMAGE_HPP

#include "ecs.h"
#include <string>

namespace Graphic {
struct Image {
public:
  int width, height, nrChannels;
  unsigned char *data = nullptr;
  bool flipVertically;
  std::string path;
  Image();
  Image(std::string path, bool flipVertically = false);
};
struct image_loader final
    : entt::resource_loader<image_loader, Graphic::Image> {
  std::shared_ptr<Graphic::Image> load(std::string path,
                                       bool flipVertically = false) const;
};
} // namespace Graphic

#endif