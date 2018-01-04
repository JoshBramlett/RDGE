#include <rdge/assets/file_formats/asset_pack.hpp>

#include <cstring>

namespace rdge {
namespace asset_pack {

std::ostream&
operator<< (std::ostream& os, asset_type value)
{
    switch (value)
    {
#define CASE(X) case X: return os << (strrchr(#X, '_') + 1); break;
        CASE(asset_type_invalid)
        CASE(asset_type_surface)
        CASE(asset_type_font)
        CASE(asset_type_spritesheet)
        CASE(asset_type_tilemap)
        CASE(asset_type_tileset)
        CASE(asset_type_sound)
        default: break;
#undef CASE
    }

    return os << "unknown[" << static_cast<uint32>(value) << "]";
}

} // namespace asset_pack
} // namespace rdge
