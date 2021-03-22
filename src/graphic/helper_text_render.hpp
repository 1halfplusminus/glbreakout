#pragma once
#include "ecs.h"
#include "freefont.h"

namespace Graphic
{
    namespace Text
    {
        struct font_face_loader final : entt::resource_loader<font_face_loader, FT_Face>
        {
            std::shared_ptr<FT_Face> load(FT_Library &ft, const std::string &path) const;
        };
    }
} // namespace Graphic
