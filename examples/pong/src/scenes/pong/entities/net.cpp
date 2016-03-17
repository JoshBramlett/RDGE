#include "net.hpp"

#include <rdge/graphics/rect.hpp>

namespace pong {

namespace {

constexpr double BASE_NET_WIDTH   = 0.0078125;     // 10px / 1280
constexpr double BASE_NET_HEIGHT  = 0.0694444;     // 50px / 720
constexpr double BASE_NET_SPACING = 0.0347222;     // 25px / 720

} // anonymous namespace

Net::Net (void)
{
    auto dimensions = RDGE::Graphics::Rect(0, 0, 1280, 720);//m_viewport.Dimensions();
    //auto dimensions = m_viewport.Dimensions();
    m_netWidth   = static_cast<RDGE::Int32>(dimensions.w * BASE_NET_WIDTH);
    m_netHeight  = static_cast<RDGE::Int32>(dimensions.h * BASE_NET_HEIGHT);
    m_netSpacing = static_cast<RDGE::Int32>(dimensions.h * BASE_NET_SPACING);

    m_netCount = dimensions.h / (m_netHeight + m_netSpacing);
}

void
Net::Render (const RDGE::Window& window)
{
    auto dimensions = RDGE::Graphics::Rect(0, 0, 1280, 720);//m_viewport.Dimensions();
    //auto dimensions = m_viewport.Dimensions();
    RDGE::UInt32 y_offset = ((dimensions.h % (m_netHeight + m_netSpacing)) + m_netSpacing) / 2;

    RDGE::UInt32 x_pos = (dimensions.w - m_netWidth) / 2;
    RDGE::UInt32 y_pos = y_offset;

    for (RDGE::Int32 i = 0; i < m_netCount; ++i)
    {
        auto r = RDGE::Graphics::Rect(x_pos, y_pos, m_netWidth, m_netHeight);
        window.DrawPrimitiveRect(&r, RDGE::Color::White());

        y_pos += m_netHeight + m_netSpacing;
    }
}

} // namespace pong
