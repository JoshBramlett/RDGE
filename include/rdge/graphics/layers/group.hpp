//! \headerfile <rdge/graphics/layers/group.hpp>
//! \author Josh Bramlett
//! \version 0.0.5
//! \date 05/17/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/math/mat4.hpp>
#include <rdge/graphics/renderable2d.hpp>
#include <rdge/graphics/renderer2d.hpp>

#include <vector>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {

//! \class Group
//! \brief Represents a group of renderable objects
//! \details Used for logical grouping and performing a similar transformation
//!          on all renderables within the collection.  Ownership of
//!          renderable objects is not transferred to the group.
class Group : public Renderable2D
{
public:
    //! \brief Group ctor
    //! \details The transformation is defaulted to an indentity matrix to
    //!          allow for logical ordering without processing a transformation.
    //! \param [in] transformation Transformation to apply
    explicit Group (const RDGE::Math::mat4& transformation = RDGE::Math::mat4::identity());

    //! \brief Group dtor
    virtual ~Group (void) { }

    //! \brief Group Copy ctor
    //! \details Non-copyable
    Group (const Group&) = delete;

    //! \brief Group Move ctor
    //! \details Transfers ownership
    Group (Group&&) noexcept;

    //! \brief Group Copy Assignment Operator
    //! \details Non-copyable
    Group& operator= (const Group&) = delete;

    //! \brief Group Move Assignment Operator
    //! \details Transfers ownership
    Group& operator= (Group&&) noexcept;

    //! \brief Add renderable to the group
    //! \param [in] renderable Pointer to renderable object
    void AddRenderable (Renderable2D* renderable);

    //! \brief Set the relative transformation
    //! \param [in] transformation Transformation matrix
    void SetTransformation (const RDGE::Math::mat4& transformation);

    //! \brief Rotates the group around the centroid
    //! \details Modifies the underlying transformation matrix
    //! \param [in] angle Angle to rotate
    void RotateOnCenter (float angle);

    //! \brief Submit all children to the renderer
    //! \param [in] renderer Renderer children will be drawn to
    virtual void Submit (Renderer2D* renderer) const override;

    //! \brief Set opacity for all children
    //! \param [in] opacity Alpha channel value (0-255)
    virtual void SetOpacity (RDGE::UInt8 opacity) override;

    //! \brief Set the Z-Index position value for all children
    //! \param [in] zindex Z-Index value
    virtual void SetZIndex (float zindex) override;

private:
    std::vector<Renderable2D*> m_children;
    RDGE::Math::mat4           m_transformation;
};

} // namespace Graphics
} // namespace RDGE
