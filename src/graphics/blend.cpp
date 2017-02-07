#include <rdge/graphics/blend.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

namespace {

// A global cache is provided to limit the amount of state changes
// TODO A more robost solution would be to query OpenGL for the current
//      state, since this is limited to this translation unit.  I'll need
//      to run a perf test to see what the cost is to query OpenGL vs
//      only enforce for this implementation.
rdge::Blend s_cached;

} // anonymous namespace

namespace rdge {

const Blend Blend::LerpSourceAlpha (BlendFactor::SourceAlpha,
                                    BlendFactor::OneMinusSourceAlpha,
                                    BlendFactor::One,
                                    BlendFactor::Zero);

const Blend Blend::LerpSourceDestAlpha (BlendFactor::SourceAlpha,
                                        BlendFactor::OneMinusSourceAlpha,
                                        BlendFactor::One,
                                        BlendFactor::OneMinusSourceAlpha);

Blend::Blend (BlendFactor src, BlendFactor dst)
    : Blend(src, dst, src, dst)
{ }

Blend::Blend (BlendFactor s_rgb, BlendFactor d_rgb, BlendFactor s_alpha, BlendFactor d_alpha)
    : src_rgb(s_rgb)
    , dst_rgb(d_rgb)
    , src_alpha(s_alpha)
    , dst_alpha(d_alpha)
{ }

void
Blend::Apply (void) const
{
    // bail if state remains disabled or if there's no change from the cached state
    if (!(s_cached.enabled || this->enabled) || s_cached == *this)
    {
        return;
    }

    if (this->enabled)
    {
        opengl::Enable(GL_BLEND);
        opengl::SetBlendFunction(static_cast<uint32>(this->src_rgb),
                                 static_cast<uint32>(this->dst_rgb),
                                 static_cast<uint32>(this->src_alpha),
                                 static_cast<uint32>(this->dst_alpha));
        opengl::SetBlendEquation(static_cast<uint32>(this->mode_rgb),
                                 static_cast<uint32>(this->mode_alpha));
    }
    else
    {
        opengl::Disable(GL_BLEND);
    }

    s_cached = *this;
}

} // namespace rdge
