// clang-format off
#include <glad/glad.h>
// clang-format on
#include "graphic_texture.hpp"
#include <iostream>
std::shared_ptr<Graphic::Texture>
Graphic::texture_image_loader::load(Image &image) const
{
  auto textureResource =
      std::shared_ptr<Graphic::Texture>(new Graphic::Texture());
  unsigned int texture;
  glGenTextures(1, &texture);
  if (image.data)
  {
    GLenum format;
    if (image.nrChannels == 1)
      format = GL_RED;
    else if (image.nrChannels == 3)
      format = GL_RGB;
    else if (image.nrChannels == 4)
      format = GL_RGBA;
    else
      format = GL_RGBA8;
    /*     std::cout << "Image format: " << image.nrChannels << "\n"; */
    // bind texture
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, image.width, image.height, 0, format,
                 GL_UNSIGNED_BYTE, image.data);

    // texture wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    // texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // texture mipmap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  textureResource->id = texture;
  textureResource->width = image.width;
  textureResource->height = image.height;
  return textureResource;
}