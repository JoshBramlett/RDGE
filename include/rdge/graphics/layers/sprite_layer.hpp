//! \headerfile <rdge/graphics/layers/sprite_layer.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/26/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/graphics/tex_coords.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/util/containers/intrusive_list.hpp>
#include <rdge/debug/widgets/graphics_widget.hpp>

#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class SpriteBatch;
class SpriteSheet;
class OrthographicCamera;
namespace tilemap { class Layer; }
//!@}

//! \enum SpriteRenderOrder
//! \brief Options for how to render the \ref SpriteLayer
enum class SpriteRenderOrder
{
    INVALID = -1,
    TOPDOWN,      //!< Sorted by y-coordinate, drawing from top to bottom
    INDEX         //!< Draws sprites in the order they are added to the layer
};

struct sprite_data : public intrusive_list_element<sprite_data>
{
    size_t index; //!< Index the sprite was added to the layer

    //!@{ Location
    math::vec2 pos;
    float depth;
    //!@}

    //!@{ Frame
    math::vec2 size;
    math::vec2 origin;
    tex_coords uvs;
    //!@}

    //!@{ Render properties
    int32 tid;
    color color;
    //!@}
};

class SpriteLayer
{
public:
    explicit SpriteLayer (uint16 capacity);
    SpriteLayer (const tilemap::Layer& def, float scale);
    ~SpriteLayer (void) noexcept;

    //!@{ Non-copyable, move enabled
    SpriteLayer (const SpriteLayer&) = delete;
    SpriteLayer& operator= (const SpriteLayer&) = delete;
    SpriteLayer (SpriteLayer&&) noexcept;
    SpriteLayer& operator= (SpriteLayer&&) noexcept;
    //!@}

    //! \brief Draw all tiles within the camera bounds
    void Draw (SpriteBatch& renderer, const OrthographicCamera& camera);

    sprite_data* AddSprite (const math::vec2& pos,
                            uint32 id,
                            const SpriteSheet& spritesheet,
                            float scale);

private:
    friend class rdge::debug::GraphicsWidget;

    intrusive_list<sprite_data> m_list;
    sprite_data* m_sprites = nullptr;
    size_t m_spriteCount = 0;
    size_t m_spriteCapacity = 0;

    color m_color = color::WHITE; //!< Render color (to store opacity)
    math::vec2 m_padding;         //!< Culling region padding

public:
    std::string name;              //!< Layer name
    std::vector<Texture> textures; //!< Sprite textures

#ifdef RDGE_DEBUG
public:
    struct debug_overlay_data
    {
        bool hide_layer = false;
        bool draw_sprite_frames = false;
        size_t sprites_drawn = 0;
    } debug_overlay;
#endif
};

//! \brief SpriteRenderOrder stream output operator
std::ostream& operator<< (std::ostream&, SpriteRenderOrder);

//!@{ SpriteRenderOrder string conversions
bool try_parse (const std::string&, SpriteRenderOrder&);
std::string to_string (SpriteRenderOrder);
//!@}

} // namespace rdge
