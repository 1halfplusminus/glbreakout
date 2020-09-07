#include "graphic_image.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

Graphic::Image::Image() {}

Graphic::Image::Image(std::string path, bool flipVertically) {
  this->path = path;
  this->flipVertically = flipVertically;
};

std::shared_ptr<Graphic::Image>
Graphic::image_loader::load(std::string path, bool flipVertically) const {
  auto image = std::shared_ptr<Graphic::Image>(new Image());
  image->path = path;
  image->flipVertically = flipVertically;
  if (image->data == nullptr) {
    stbi_set_flip_vertically_on_load(image->flipVertically);
    image->data = stbi_load(image->path.c_str(), &image->width, &image->height,
                            &image->nrChannels, 0);
    if (!image->data) {
      std::cout << "Cubemap texture failed to load at path: "
                << image->path.c_str() << std::endl;
      stbi_image_free(image->data);
    }
    /*       stbi_image_free(image.data); */
  }
  return image;
}