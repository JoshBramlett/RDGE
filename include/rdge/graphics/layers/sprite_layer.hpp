//! \headerfile <rdge/graphics/layers/sprite_layer.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/26/2016

#pragma once

#include <rdge/core.hpp>

#include <memory>
#include <vector>

// NewSpriteLayer
#include <rdge/util/containers/intrusive_list.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/graphics/tex_coords.hpp>
#include <rdge/graphics/texture.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//!@{ Forward declarations
class ISprite;
class Shader;
class SpriteBatch;
//!@}

// NewSpriteLayer
//!@{ Forward declarations
//class SpriteBatch;
class SpriteSheet;
class OrthographicCamera;
struct spritesheet_region;
namespace tilemap { class Layer; }
//!@}

//! \enum DrawOrder
//! \brief Options for how to render the \ref SpriteLayer
enum class DrawOrder
{
    INVALID = -1,
    TOPDOWN,      //!< Sorted by y-coordinate, drawing from top to bottom
    INDEX         //!< Draws sprites in the order they are added to the layer
};

//! \class SpriteLayer
//! \brief Layer of ISprite objects
//! \details A layer represents a logical group of sprites that will be drawn
//!          together using the same render target.  A uniform depth value can be set
//!          for all cached sprites in order to mimic the behavior of a photoshop
//!          layer.  Facilitates the interface between the sprite and the render
//!          target for batch drawing.
class SpriteLayer
{
public:
    //! \brief SpriteLayer default ctor
    //! \details Creates a new instance of the render target.
    //! \param [in] num_sprites Max number of sprites that can be submitted
    //! \param [in] shader Shader to pass to the render target
    explicit SpriteLayer (uint16 num_sprites = 1000, std::shared_ptr<Shader> shader = nullptr);

    //! \brief SpriteLayer ctor
    //! \details Uses an existing render target.
    //! \param [in] render_target Shared render target
    // TODO If multiple layers share a renderer each still does it own prep and flush.
    //      I could easily avoid this by setting a flag on the draw call.  Make sure
    //      to profile the impact.
    explicit SpriteLayer (std::shared_ptr<SpriteBatch> render_target);

    //! \brief SpriteLayer dtor
    ~SpriteLayer (void) noexcept = default;

    //!@{ Non-copyable, move enabled
    SpriteLayer (const SpriteLayer&) = delete;
    SpriteLayer& operator= (const SpriteLayer&) = delete;
    SpriteLayer (SpriteLayer&&) noexcept = default;
    SpriteLayer& operator= (SpriteLayer&&) noexcept = default;
    //!@}

    //! \brief Add sprite to the cache
    //! \param [in] sprite Shared ISprite object
    void AddSprite (std::shared_ptr<ISprite> sprite);

    //! \brief Draw all cached sprites
    void Draw (void);

    //! \brief Give all cached sprites a uniform depth
    //! \param [in] depth Depth (z-index) value
    void OverrideSpriteDepth (float depth);

public:
    std::shared_ptr<SpriteBatch>          renderer; //!< Render target
    std::vector<std::shared_ptr<ISprite>> sprites;  //!< Collection of sprites
};

struct sprite_data : public intrusive_list_element<sprite_data>
{
    size_t index;
    math::vec2 pos;
    math::vec2 size;
    math::vec2 origin;
    float depth;
    tex_coords uvs;
    int32 tid;
    color color;
};

class NewSpriteLayer
{
public:
    explicit NewSpriteLayer (const tilemap::Layer& def,
                             const SpriteSheet& spritesheet,
                             float scale);
    ~NewSpriteLayer (void) noexcept;

    //!@{ Non-copyable, move enabled
    NewSpriteLayer (const NewSpriteLayer&) = delete;
    NewSpriteLayer& operator= (const NewSpriteLayer&) = delete;
    NewSpriteLayer (NewSpriteLayer&&) noexcept;
    NewSpriteLayer& operator= (NewSpriteLayer&&) noexcept;
    //!@}

    // thoughts:
    // Good idea to pre-sort b/c most sprites will be static and should therefore
    // have cache locality when rendering.
    //! \brief Draw all tiles within the camera bounds
    void Draw (SpriteBatch& renderer, const OrthographicCamera& camera);

    //sprite_data* AddSprite (const math::vec2 pos,
                            //const spritesheet_region& region,
                            //Texture texture);

    //void BoundsDirty (sprite_data* data);

private:
    intrusive_list<sprite_data> m_list;
    sprite_data* m_sprites = nullptr;
    size_t m_spriteCount = 0;

    color m_color = color::WHITE;  //!< Render color (to store opacity)

public:
    std::vector<Texture> textures; //!< Sprite textures
};

//! \brief DrawOrder stream output operator
std::ostream& operator<< (std::ostream&, DrawOrder);

//!@{ DrawOrder string conversions
bool try_parse (const std::string&, DrawOrder&);
std::string to_string (DrawOrder);
//!@}

} // namespace rdge
