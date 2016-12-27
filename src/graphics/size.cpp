#include <rdge/graphics/size.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

namespace rdge {

math::vec2
to_ndc (const size& size)
{
    auto viewport = opengl::GetViewport();
    return { viewport[2] * (size.w / viewport[2]),
             viewport[3] * (size.h / viewport[3]) };
}

} // namespace rdge
