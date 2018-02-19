#include <rdge/assets/file_formats/asset_pack.hpp>

#include <cstring>
#include <sstream>

namespace rdge {
namespace asset_pack {

std::ostream&
operator<< (std::ostream& os, asset_type value)
{
    return os << rdge::to_string(value);
}

} // namespace asset_pack

std::string
to_string (asset_pack::asset_type value)
{
    switch (value)
    {
#define CASE(X) case X: return (strrchr(#X, '_') + 1); break;
        CASE(asset_pack::asset_type_invalid)
        CASE(asset_pack::asset_type_surface)
        CASE(asset_pack::asset_type_font)
        CASE(asset_pack::asset_type_spritesheet)
        CASE(asset_pack::asset_type_tilemap)
        CASE(asset_pack::asset_type_tileset)
        CASE(asset_pack::asset_type_sound)
        default: break;
#undef CASE
    }

    std::ostringstream ss;
    ss << "unknown[" << static_cast<uint32>(value) << "]";
    return ss.str();
}

} // namespace rdge
