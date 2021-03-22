#include "freefont.h"
#include "helper_text_render.hpp"
#include <iostream>
std::shared_ptr<FT_Face>
Graphic::Text::font_face_loader::load(FT_Library &ft, const std::string &path) const
{
    FT_Face *face = new FT_Face;
    if (FT_New_Face(ft, path.c_str(), 0, face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font at:" << path << std::endl;
        exit(-1);
    }
    return std::shared_ptr<FT_Face>(face);
}