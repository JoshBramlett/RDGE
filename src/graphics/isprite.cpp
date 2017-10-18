#include <rdge/graphics/isprite.hpp>
#include <rdge/util/strings.hpp>

namespace rdge {

std::ostream& operator<< (std::ostream& os, const SpriteVertices& value)
{
    os << "SpriteVertices: [";

    if (value[0].tid == Texture::INVALID_UNIT_ID)
    {
       os << "\n  tid=INVALID";
    }
    else
    {
       os << "\n  tid=" << value[0].tid;
    }

    os << "\n  color=" << print_hex(value[0].color)
       << "\n  [0] pos=" << value[0].pos << " uv=" << value[0].uv
       << "\n  [1] pos=" << value[1].pos << " uv=" << value[1].uv
       << "\n  [2] pos=" << value[2].pos << " uv=" << value[2].uv
       << "\n  [3] pos=" << value[3].pos << " uv=" << value[3].uv;

    return os << "\n]\n";
}

} // namespace rdge
